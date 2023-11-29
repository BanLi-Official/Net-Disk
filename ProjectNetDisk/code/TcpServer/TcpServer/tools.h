#ifndef TOOLS_H
#define TOOLS_H
#include "protocol.h"






class Tools
{
public:
    Tools();
    void ShowPDU(PDU *pdu);
    static Tools getInstance();
};

#endif // TOOLS_H
