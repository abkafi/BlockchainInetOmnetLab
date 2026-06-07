Blockchain Simulation using OMNeT++ 6.4 and INET 4.6
Step-by-Step Laboratory Manual with Theory, Descriptions, and Code Snippets
Lab 1: Minimal UDP Application
Objective: Learn ApplicationBase, UdpSocket, packet transmission and packet reception.
Brief Description:
A UDP application is the foundation of the blockchain simulator. Students learn how to bind a socket, send packets and receive packets using the INET framework.
Code Snippet:
// Send a packet
auto packet = new Packet("Hello");
socket.sendTo(packet, destinationAddress, destinationPort);

// Receive a packet
void socketDataArrived(UdpSocket *socket, Packet *packet)
{
    EV << "Received " << packet->getName() << endl;
    delete packet;
}
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
Lab 2: Echo Application
Objective: Implement request-response communication.
Brief Description:
Host1 sends a packet to Host2. Host2 receives it and sends an echo reply back. This introduces client-server communication.
Code Snippet:
// Host2 echoes back
auto echoPacket = new Packet("EchoKafi");
socket->sendTo(echoPacket, destinationAddress, destinationPort);
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
Lab 3: BlockAdd (Transaction Message)
Objective: Create blockchain transactions using FieldsChunk.
Brief Description:
Blockchain transactions are represented as structured messages rather than plain packet names.
Code Snippet:
auto tx = makeShared<TransactionChunk>();
tx->setTxId(1);
tx->setSender("Alice");
tx->setReceiver("Bob");
tx->setAmount(100.5);

auto packet = new Packet("Transaction");
packet->insertAtBack(tx);
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
Lab 4: MempoolApp
Objective: Store incoming transactions.
Brief Description:
A mempool stores pending transactions before they are included in a block.
Code Snippet:
struct TransactionInfo {
    int txId;
    std::string sender;
    std::string receiver;
    double amount;
};

std::vector<TransactionInfo> mempool;
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
Lab 5: BlockChunk
Objective: Represent blockchain blocks.
Brief Description:
Transactions are grouped into blocks. Each block contains metadata such as block ID and previous hash.
Code Snippet:
auto block = makeShared<BlockChunk>();
block->setBlockId(1);
block->setPrevHash(0);
block->setNonce(1234);
block->setTxCount(3);
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
Lab 6: BlockchainApp
Objective: Implement blockchain node behavior.
Brief Description:
A blockchain node receives transactions, stores them in a mempool, mines blocks, and propagates blocks.
Code Snippet:
if (mempool.size() >= 3)
{
    createBlock();
}
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
Lab 7: Multi-Node Blockchain Network
Objective: Create a distributed blockchain network.
Brief Description:
Multiple nodes exchange transactions and blocks to maintain a synchronized blockchain.
Code Snippet:
for (auto addr : peers)
{
    socket.sendTo(packet->dup(), addr, destinationPort);
}
Expected Result: Students should observe the successful execution of the described functionality in the simulation output window.
