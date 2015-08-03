/***
    This file is part of snapcast
    Copyright (C) 2015  Johannes Pohl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef CONTROL_SERVER_H
#define CONTROL_SERVER_H

#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <memory>
#include <set>
#include <sstream>
#include <mutex>

#include "serverSession.h"
#include "pipeReader.h"
#include "common/queue.h"
#include "message/message.h"
#include "message/header.h"
#include "message/sampleFormat.h"
#include "message/serverSettings.h"


using boost::asio::ip::tcp;
typedef std::shared_ptr<tcp::socket> socket_ptr;
using namespace std;


struct ControlServerSettings
{
	size_t port;
	std::string fifoName;
	std::string codec;
	int32_t bufferMs;
	msg::SampleFormat sampleFormat;
};


class ControlServer : public MessageReceiver, PipeListener
{
public:
	ControlServer(const ControlServerSettings& controlServerSettings);

	void start();
	void stop();
	void send(const msg::BaseMessage* message);
	virtual void onMessageReceived(ServerSession* connection, const msg::BaseMessage& baseMessage, char* buffer);
	virtual void onChunkRead(const PipeReader* pipeReader, const msg::PcmChunk* chunk);
	virtual void onResync(const PipeReader* pipeReader, double ms);

private:
	void startAccept();
	void handleAccept(socket_ptr socket);
	void acceptor();
	mutable std::mutex mutex_;
	PipeReader* pipeReader_;
	set<shared_ptr<ServerSession>> sessions_;
	boost::asio::io_service io_service_;
	shared_ptr<tcp::acceptor> acceptor_;

	ControlServerSettings settings_;
	msg::SampleFormat sampleFormat_;
	msg::ServerSettings serverSettings_;
	thread acceptThread_;
	Queue<shared_ptr<msg::BaseMessage>> messages_;
};



#endif


