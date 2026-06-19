#ifndef __BLOCKCHAINAPP_H
#define __BLOCKCHAINAPP_H

#include "inet/applications/base/ApplicationBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include <vector>
#include <string>



using namespace inet;
struct TransactionInfo
{
    int txId;
    std::string sender;
    std::string receiver;
    double amount;
};

struct Block
{
    int blockId;

        int previousBlockHash;

        simtime_t timestamp;

        int blockHash;

        std::vector<TransactionInfo> transactions;
};

class BlockchainApp : public ApplicationBase, public UdpSocket::ICallback
{
  protected:
    UdpSocket socket;

    int localPort;
    int destPort;

    L3Address destAddress;

    cMessage *sendTimer = nullptr;
    cMessage *mineTimer = nullptr;
    int nodeId;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;

    void sendTransaction();
    void mineBlock();
    void printMempool();
    void printBlockchain();
    void broadcastBlock(
            int blockId,
            int prevHash,
            int hash,
            int txCount);

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override {}
    virtual void socketClosed(UdpSocket *socket) override {}
    bool validateBlock(int prevHash);

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    std::vector<TransactionInfo> mempool;
    std::vector<Block> blockchain;
    int txCounter = 1;
    int nextBlockId = 1;
};

#endif
