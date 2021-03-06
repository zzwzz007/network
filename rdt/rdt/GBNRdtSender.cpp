#include <cmath>
#include "GBNRdtSender.h"
#include "DataStructure.h"
#include "stdafx.h"
#include "Global.h"

GBNRdtSender::GBNRdtSender() :
	winSize(4), seqSize(8), sndPkt(new Packet[8])
{
	base = 1;
	nextSeqnum = 1;
}

GBNRdtSender::~GBNRdtSender()
{
	delete[] sndPkt;
}
//判断缓冲区有无位置
bool GBNRdtSender::getWaitingState()
{
	return (base + winSize) % seqSize == nextSeqnum % seqSize;
}
//发送
bool GBNRdtSender::send(const Message& message)
{
	if (getWaitingState())
	{
		cout << "发送窗口已满！\n";
		return false;
	}

	sndPkt[nextSeqnum].acknum = -1; //忽略该字段
	sndPkt[nextSeqnum].seqnum = nextSeqnum;
	memcpy(sndPkt[nextSeqnum].payload, message.data, sizeof(message.data));
	sndPkt[nextSeqnum].checksum = pUtils->calculateCheckSum(sndPkt[nextSeqnum]);
	pUtils->printPacket("发送方发送报文：", sndPkt[nextSeqnum]);
	if (base == nextSeqnum)
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	pns->sendToNetworkLayer(RECEIVER, sndPkt[nextSeqnum]);
	nextSeqnum = (nextSeqnum + 1) % seqSize;
	return true;
}
//接收
void GBNRdtSender::receive(const Packet& ackPkt)
{
	int checknum = pUtils->calculateCheckSum(ackPkt);
	if (checknum != ackPkt.checksum)
		pUtils->printPacket("接收ack损坏！", ackPkt);
	else
	{
		base = (ackPkt.acknum + 1) % seqSize;
		pns->stopTimer(SENDER, 0);
		if (base != nextSeqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		cout << "窗口滑动\n";
	}
}
//超时
void GBNRdtSender::timeoutHandler(int seqNum)
{
	cout << "超时\n";
	pns->stopTimer(SENDER, 0);
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	for (int i = base; i != nextSeqnum; i = (i + 1) % seqSize)
	{
		pUtils->printPacket("重发报文：", sndPkt[i]);
		pns->sendToNetworkLayer(RECEIVER, sndPkt[i]);
	}
	cout << "重发完成！\n";
}