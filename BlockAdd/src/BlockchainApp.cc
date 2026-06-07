#include "inet/networklayer/common/L3AddressResolver.h"
#include "BlockchainApp.h"
#include "TransactionChunk_m.h"
#include <cstring>

Define_Module(BlockchainApp);

void BlockchainApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_APPLICATION_LAYER) {

        localPort = par("localPort");
        destPort = par("destPort");

        const char *hostName = getParentModule()->getFullName();

        if (strcmp(hostName, "host1") == 0)
            nodeId = 1;
        else if (strcmp(hostName, "host2") == 0)
            nodeId = 2;
        else
            nodeId = 999;

        destAddress =
            L3AddressResolver().resolve(par("destAddress"));

        socket.setOutputGate(gate("socketOut"));
        socket.bind(localPort);

        socket.setCallback(this);


        sendTimer = new cMessage("sendTimer");
        scheduleAt(simTime() + 1, sendTimer);
        mineTimer = new cMessage("mineTimer");
        scheduleAt(simTime() + 10, mineTimer);
        EV_INFO << "Node ID = "
                << nodeId
                << endl;
    }
}

void BlockchainApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg == sendTimer) {

        sendTransaction();

        scheduleAt(simTime() + 5, sendTimer);
    }
    else if (msg == mineTimer) {

        mineBlock();

        scheduleAt(simTime() + 10, mineTimer);
    }
    else {
        socket.processMessage(msg);
    }
}

void BlockchainApp::sendTransaction()
{
    auto packet = new Packet("Transaction");

    const auto& tx = makeShared<TransactionChunk>();

    int globalTxId = nodeId * 1000000 + txCounter++;

    tx->setTxId(globalTxId);
    tx->setSender(getParentModule()->getFullName());
    tx->setReceiver("Bob");
    tx->setAmount(50.0);

    tx->setChunkLength(B(64));

    TransactionInfo info;

    info.txId = tx->getTxId();
    info.sender = tx->getSender();
    info.receiver = tx->getReceiver();
    info.amount = tx->getAmount();

    mempool.push_back(info);

    packet->insertAtBack(tx);

    EV_INFO << "Host = "
            << getParentModule()->getFullName()
            << endl;

    socket.sendTo(packet, destAddress, destPort);
}

void BlockchainApp::socketDataArrived(UdpSocket *socket,
                                      Packet *packet)
{
    const auto& tx =
        packet->peekAtFront<TransactionChunk>();
    TransactionInfo info;

    info.txId = tx->getTxId();
    info.sender = tx->getSender();
    info.receiver = tx->getReceiver();
    info.amount = tx->getAmount();

    mempool.push_back(info);
    printMempool();

//    EV_INFO << "Mempool size = "
//            << mempool.size()
//            << endl;
//
//    EV_INFO << "Received Transaction\n"
//            << "TxId     = " << tx->getTxId() << "\n"
//            << "Sender   = " << tx->getSender() << "\n"
//            << "Receiver = " << tx->getReceiver() << "\n"
//            << "Amount   = " << tx->getAmount()
//            << endl;

    delete packet;
}
void BlockchainApp::printMempool()
{
    EV_INFO << "\n=== MEMPOOL ===\n";

    for (const auto& tx : mempool) {
        EV_INFO << "TxId="
                << tx.txId
                << " "
                << tx.sender
                << "->"
                << tx.receiver
                << " Amount="
                << tx.amount
                << "\n";
    }

    EV_INFO << "===============\n";
}
void BlockchainApp::mineBlock()
{
    if (mempool.empty()) {
           EV_INFO << "Nothing to mine\n";
           return;
       }

       Block block;
       block.timestamp = simTime();

       block.blockId = nextBlockId++;

       if (blockchain.empty())
           block.previousBlockHash = 0;
       else
           block.previousBlockHash =
               blockchain.back().blockHash;
       block.transactions = mempool;
       block.blockHash =
               block.blockId +
               block.previousBlockHash +
               block.transactions.size();


       blockchain.push_back(block);

       printBlockchain();

       mempool.clear();



}
void BlockchainApp::printBlockchain()
{
    EV_INFO << "\n=== BLOCKCHAIN ===\n";

        for (const auto& block : blockchain) {

            EV_INFO << "\nBlock "
                    << block.blockId
                    << " (Prev="
                    << block.previousBlockHash
                    << ", Time="
                    << block.timestamp
                    << ", Hash="
                    << block.blockHash
                    << ")\n";

            for (const auto& tx : block.transactions) {

                EV_INFO << "  TxId="
                        << tx.txId
                        << " "
                        << tx.sender
                        << "->"
                        << tx.receiver
                        << " Amount="
                        << tx.amount
                        << "\n";
            }
        }

        EV_INFO << "==================\n";
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
