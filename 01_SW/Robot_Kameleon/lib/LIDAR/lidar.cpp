#include "lidar.hpp"

Lidar::Lidar(PinName tx, PinName rx, int baud)
    : serial(tx, rx, baud), buffer_pos(0)
{
    serial.attach(callback(this, &Lidar::on_uart_rx), SerialBase::RxIrq);
    lidar_thread.start(callback(&queue, &EventQueue::dispatch_forever));
    Lidar_print_thread.start(callback(this, &Lidar::routine_lidar));
}

void Lidar::on_uart_rx()
{
    // Lecture des données dispo
    while (serial.readable()) {
        uint8_t c;
        if (serial.read(&c, 1) == 1) {
            if (buffer_pos < sizeof(buffer)) {
                buffer[buffer_pos++] = c;
            }
            
            if (buffer_pos >= DATA_PACKET_SIZE) {
                queue.call(callback(this, &Lidar::processBuffer));
            }
        }
    }
}

void Lidar::processBuffer()
{
    // Chercher début de paquet (header + ver_len)
    size_t start = 0;
    bool found = false;
    while (start + DATA_PACKET_SIZE <= buffer_pos) {
        if (buffer[start] == HEADER && buffer[start + 1] == VERLEN) {
            found = true;
            break;
        }
        ++start;
    }

    if (!found) {
        // Pas trouvé, vider buffer partiellement
        if (buffer_pos > DATA_PACKET_SIZE) {
            memmove(buffer, buffer + buffer_pos - DATA_PACKET_SIZE, DATA_PACKET_SIZE);
            buffer_pos = DATA_PACKET_SIZE;
        }
        return;
    }

    // Si paquet complet dispo
    if (start + DATA_PACKET_SIZE <= buffer_pos) {
        parsePacket(buffer + start);

        // Décaler le buffer
        size_t new_pos = buffer_pos - (start + DATA_PACKET_SIZE);
        if (new_pos > 0) {
            memmove(buffer, buffer + start + DATA_PACKET_SIZE, new_pos);
        }
        buffer_pos = new_pos;
    }
}

void Lidar::parsePacket(const uint8_t* DataPacket)
{
    _dataPacket.header      = DataPacket[0];
    _dataPacket.ver_len     = DataPacket[1];
    _dataPacket.speed       = ((uint16_t)DataPacket[3] << 8) | DataPacket[2];
    _dataPacket.start_angle = ((uint16_t)DataPacket[5] << 8) | DataPacket[4];
    _dataPacket.end_angle   = ((uint16_t)DataPacket[43] << 8) | DataPacket[42];
    _dataPacket.timestamp   = ((uint16_t)DataPacket[45] << 8) | DataPacket[44];
    _dataPacket.crc8        = DataPacket[46];

    float step = 0.0f;
    int delta = (int)_dataPacket.end_angle - (int)_dataPacket.start_angle;

    if (delta > 0)
        step = float(delta) / float(POINT_PER_PACK - 1);
    else
        step = float((36000 + delta)) / float(POINT_PER_PACK - 1);

    for (uint8_t i = 0; i < POINT_PER_PACK; ++i) {
        uint8_t offset = 6 + 3 * i;
        _dataPacket.point[i].distance  = ((uint16_t)DataPacket[offset + 1] << 8) | DataPacket[offset];
        _dataPacket.point[i].intensity = DataPacket[offset + 2];

        float angle = _dataPacket.start_angle + (step * i);
        _dataPacket.point[i].angle = int(angle) % 36000;
    }
}

LiDARFrameTypeDef Lidar::GetPoints(void)
{
    return _dataPacket;
}

void Lidar::routine_lidar(void)
{
  while (true) {
    // Récupérer la dernière trame
    LiDARFrameTypeDef frame = Lidar::GetPoints();

    // Affichage simple des distances
    printf("Start angle: %.2f deg\n", frame.start_angle / 100.0f);
    for (int i = 0; i < POINT_PER_PACK; i++) {
        printf("Point %d: angle=%.2f deg, distance=%d mm, intensity=%d\n",
                i,
                frame.point[i].angle / 100.0f,
                frame.point[i].distance,
                frame.point[i].intensity);
    }
    printf("-----\n");

    ThisThread::sleep_for(1ms); 
  }   
}