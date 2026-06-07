#include "inet/networklayer/common/L3AddressResolver.h"
#include "BlockchainApp.h"
#include "TransactionChunk_m.h"

Define_Module(BlockchainApp);

void BlockchainApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_APPLICATION_LAYER) {

        localPort = par("localPort");
        destPort = par("destPort");

        destAddress =
            L3AddressResolver().resolve(par("destAddress"));

        socket.setOutputGate(gate("socketOut"));
        socket.bind(localPort);

        socket.setCallback(this);

        sendTimer = new cMessage("sendTimer");

        scheduleAt(simTime() + 1, sendTimer);
    }
}

void BlockchainApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {

        sendTransaction();

        scheduleAt(simTime() + 5, sendTimer);
    }
    else {
        socket.processMessage(msg);
    }
}

void BlockchainApp::sendTransaction()
{
    auto packet = new Packet("Transaction");

    const auto& tx = makeShared<TransactionChunk>();

    tx->setTxId(txCounter++);
    tx->setSender("Alice");
    tx->setReceiver("Bob");
    tx->setAmount(50.0);

    tx->setChunkLength(B(64));

    packet->insertAtBack(tx);

    EV_INFO << "Sending Tx "
            << tx->getTxId()
            << endl;

    socket.sendTo(packet, destAddress, destPort);
}

void BlockchainApp::socketDataArrived(UdpSocket *socket,
                                      Packet *packet)
{
    const auto& tx =
        packet->peekAtFront<TransactionChunk>();

    EV_INFO << "Received Transaction\n"
            << "TxId     = " << tx->getTxId() << "\n"
            << "Sender   = " << tx->getSender() << "\n"
            << "Receiver = " << tx->getReceiver() << "\n"
            << "Amount   = " << tx->getAmount()
            << endl;

    delete packet;
}

void BlockchainApp::finish()
{
    cancelAndDelete(sendTimer);
}
void BlockchainApp::handleStartOperation(LifecycleOperation *operation)
{
}

void BlockchainApp::handleStopOperation(LifecycleOperation *operation)
{
}

void BlockchainApp::handleCrashOperation(LifecycleOperation *operation)
{
}
