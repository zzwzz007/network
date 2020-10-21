#pragma once
#include "RdtSender.h"

class GBNRdtSender :public RdtSender
{
private:
	int base;
	int nextSeqnum;
	const int winSize;	//滑动窗口大小
	const int seqSize;	//序号大小
	Packet* const sndPkt;

public:
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkService调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待确认状态或发送窗口已满而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkService调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkService调用
	bool getWaitingState();								//返回RdtSender是否处于等待状态，如果发送方正等待确认或者发送窗口已满，返回true

public:
	GBNRdtSender();
	virtual ~GBNRdtSender();
};