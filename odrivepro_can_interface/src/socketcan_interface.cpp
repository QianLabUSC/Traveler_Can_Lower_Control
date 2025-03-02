
#include "odrivepro_can_includes/socketcan_interface.hpp"

SocketcanInterface::SocketcanInterface(canid_t msg_id)
{
    // Constructor for no mask definition
    this->openSocket(msg_id,DEFAULT_CAN_CHANNEL,DEFAULT_CANID_MASK, DEFAULT_READ_TIMOUT);
}
SocketcanInterface::SocketcanInterface(canid_t msg_id,uint8_t can_channel)
{
    // Constructor for no mask definition
    this->openSocket(msg_id,can_channel,DEFAULT_CANID_MASK, DEFAULT_READ_TIMOUT);
}
SocketcanInterface::SocketcanInterface(canid_t msg_id, uint8_t can_channel, uint16_t mask)
{
    // Constructor with mask definition
    this->openSocket(msg_id, can_channel, mask, DEFAULT_READ_TIMOUT);
}

SocketcanInterface::SocketcanInterface(canid_t msg_id, uint8_t can_channel, uint16_t mask, uint32_t timeout_microseconds)
{
    // Constructor with mask definition
    this->openSocket(msg_id, can_channel, mask, timeout_microseconds);
}

SocketcanInterface::~SocketcanInterface()
{
    // This might not ever be run
    close(this->s);
}

int SocketcanInterface::openSocket(canid_t msg_id, uint8_t can_channel, uint16_t mask, uint32_t timeout_microseconds)
{
    // Open socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        return -1;
    }
        if (can_channel == 0)
        {
            strcpy(ifr.ifr_name, "can0");
            ioctl(s, SIOCGIFINDEX, &ifr);
        }
        else 
        {
            strcpy(ifr.ifr_name, "can1");
            ioctl(s, SIOCGIFINDEX, &ifr);
        }
    can_filter filt;
    filt.can_id = msg_id;
    filt.can_mask = mask & CAN_SFF_MASK; // Mask to first 5 bits of 11 bit identifier
    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &filt, sizeof(filt)) < 0)
    {
        printf("setsockopt failed\n");
    }

    int buf_size = 32;
    if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) < 0)
    {
        printf("setsockopt failed\n");
    }

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_microseconds;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
    {
        printf("Set CAN socket option 'SO_RCVTIMEO' failed !\n");
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    int err = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0)
    {
        printf("Socket Bind Error\n");
        return -1;
    }

    return s;
}

int SocketcanInterface::readFrame(can_frame *frame)
{
    return read(this->s, frame, sizeof(*frame));
}

int SocketcanInterface::writeFrame(can_frame frame)
{
    return write(this->s, &frame, sizeof(frame));
}
