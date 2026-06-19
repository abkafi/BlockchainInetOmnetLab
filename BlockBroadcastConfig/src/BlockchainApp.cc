#include "inet/networklayer/common/L3AddressResolver.h"
#include "BlockchainApp.h"
#include "TransactionChunk_m.h"
#include "BlockChunk_m.h"
#include <cstring>

Define_Module(BlockchainApp);

void BlockchainApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_APPLICATION_LAYER) {

        localPort = par("localPort");
        destPort = par("destPort");

        nodeId = getParentModule()->getIndex();

        networkSize = par("networkSize");

//        const char *hostName = getParentModule()->getFullName();
//
//        if (strcmp(hostName, "host1") == 0)
//            nodeId = 1;
//        else if (strcmp(hostName, "host2") == 0)
//            nodeId = 2;
//        else
//            nodeId = 999;

//        destAddress =
//            L3AddressResolver().resolve(par("destAddress"));

        for (int i = 0; i < networkSize; i++)
        {
            if (i == nodeId)
                continue;

            std::string peerName =
                "host[" +
                std::to_string(i) +
                "]";

            peers.push_back(
                L3AddressResolver().resolve(
                    peerName.c_str()));
        }

//        if (strcmp(hostName, "host1") == 0)
//        {
//            peers.push_back(
//                L3AddressResolver().resolve("host2"));
//
//            peers.push_back(
//                L3AddressResolver().resolve("host3"));
//        }
//        else if (strcmp(hostName, "host2") == 0)
//        {
//            peers.push_back(
//                L3AddressResolver().resolve("host1"));
//
//            peers.push_back(
//                L3AddressResolver().resolve("host3"));
//        }
//        else if (strcmp(hostName, "host3") == 0)
//        {
//            peers.push_back(
//                L3AddressResolver().resolve("host1"));
//
//            peers.push_back(
//                L3AddressResolver().resolve("host2"));
//        }
        socket.setOutputGate(gate("socketOut"));
        socket.bind(localPort);

        socket.setCallback(this);


        sendTimer = new cMessage("sendTimer");
        scheduleAt(simTime() + 1, sendTimer);
//        mineTimer = new cMessage("mineTimer");
//        scheduleAt(simTime() + 10, mineTimer);
        // Only host1 mines blocks
        isMiner = par("isMiner");
        miningInterval = par("miningInterval");
        if (nodeId == 1) {
            mineTimer = new cMessage("mineTimer");
            scheduleAt(simTime() + 10, mineTimer);
        }
        EV_INFO << "Node ID = "
                << nodeId
                << endl;
    }
}

void BlockchainApp::handleMessageWhenUp(cMessage *msg)
{
    EV_INFO << "handleMessageWhenUp(): "
                << msg->getName()
                << endl;

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

    int globalTxId =
        (nodeId + 1) * 1000000 +
        txCounter++;

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

    //socket.sendTo(packet, destAddress, destPort);

    if (peers.empty()) {
        EV_ERROR << "No peers configured\n";
        delete packet;
        return;
    }

    int index = intuniform(0, peers.size() - 1);

    socket.sendTo(packet,
                  peers[index],
                  destPort);
}

void BlockchainApp::socketDataArrived(
        UdpSocket *socket,
        Packet *packet)
{
    EV_INFO << "socketDataArrived(): "
            << packet->getName()
            << endl;
    if (strcmp(packet->getName(), "Block") == 0)
    {
        const auto& block =
            packet->peekAtFront<BlockChunk>();

        EV_INFO << "\n";
        EV_INFO << "=== RECEIVED BLOCK ===\n";

        EV_INFO << "Block ID = "
                << block->getBlockId()
                << "\n";

        EV_INFO << "Prev Hash = "
                << block->getPrevHash()
                << "\n";

        EV_INFO << "Hash = "
                << block->getHash()
                << "\n";

        EV_INFO << "Tx Count = "
                << block->getTxCount()
                << "\n";

        bool valid =
            validateBlock(
                block->getPrevHash());

        if (valid)
        {
            EV_INFO << "VALID BLOCK\n";

            Block receivedBlock;

            receivedBlock.blockId =
                block->getBlockId();

            receivedBlock.previousBlockHash =
                block->getPrevHash();

            receivedBlock.blockHash =
                block->getHash();

            receivedBlock.timestamp =
                simTime();

            blockchain.push_back(receivedBlock);

            EV_INFO << "BLOCK ADDED TO LOCAL CHAIN\n";

            printBlockchain();
        }
        else
        {
            EV_INFO << "INVALID BLOCK\n";
        }

        delete packet;
        return;
    }

    const auto& tx =
        packet->peekAtFront<TransactionChunk>();

    TransactionInfo info;

    info.txId = tx->getTxId();
    info.sender = tx->getSender();
    info.receiver = tx->getReceiver();
    info.amount = tx->getAmount();

    mempool.push_back(info);

    printMempool();

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

       broadcastBlockToPeers(
           block.blockId,
           block.previousBlockHash,
           block.blockHash,
           block.transactions.size());

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
void BlockchainApp::broadcastBlock(
                int blockId,
                int prevHash,
                int hash,
                int txCount)
        {
    EV_INFO << "Sending block from "
            << getParentModule()->getFullName()
            << " to "
            << destAddress
            << " port "
            << destPort
            << endl;
            auto block = makeShared<BlockChunk>();

            block->setBlockId(blockId);
            block->setPrevHash(prevHash);
            block->setHash(hash);
            block->setTxCount(txCount);

            block->setChunkLength(B(32));

            auto packet = new Packet("Block");

            packet->insertAtBack(block);

            socket.sendTo(packet,
                          destAddress,
                          destPort);

            EV_INFO << "BLOCK BROADCASTED: "
                    << blockId
                    << endl;
        }
bool BlockchainApp::validateBlock(int prevHash)
{
    if (blockchain.empty())
        return (prevHash == 0);

    return (prevHash ==
            blockchain.back().blockHash);
}
void BlockchainApp::broadcastBlockToPeers(
        int blockId,
        int prevHash,
        int hash,
        int txCount)
{
    for (auto peer : peers)
    {
        auto block =
            makeShared<BlockChunk>();

        block->setBlockId(blockId);
        block->setPrevHash(prevHash);
        block->setHash(hash);
        block->setTxCount(txCount);

        block->setChunkLength(B(32));

        auto packet =
            new Packet("Block");

        packet->insertAtBack(block);

        socket.sendTo(
            packet,
            peer,
            destPort);
    }
}

void BlockchainApp::finish()
{
    if (sendTimer)
           cancelAndDelete(sendTimer);

       if (mineTimer)
           cancelAndDelete(mineTimer);
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
