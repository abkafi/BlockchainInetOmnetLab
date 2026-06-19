//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package blockreceive.simulations;


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.ethernet.EthernetSwitch;


network BlockchainNetwork
{
    parameters:
        int numHosts = default(5);

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=100,50");
        }

        router1: Router {
            @display("p=500,200");
        }
        router2: Router {
            @display("p=491,79");
        }
        switch1: EthernetSwitch {
            @display("p=300,200");
        }
        switch2: EthernetSwitch {
            @display("p=300,78");
        }

        host[numHosts]: StandardHost {
            @display("i=device/laptop");
        }

    connections allowunconnected:

        switch1.ethg++ <--> Eth100M <--> router1.ethg++;
        switch2.ethg++ <--> Eth100M <--> router2.ethg++;
        router1.ethg++ <--> Eth100M <--> router2.ethg++;

         // First half of hosts
        for i=0..int((numHosts/2))-1 {
            host[i].ethg++ <--> Eth100M <--> switch1.ethg++;
        }

        // Second half of hosts
        for i=int((numHosts/2))..numHosts-1 {
            host[i].ethg++ <--> Eth100M <--> switch2.ethg++;
        }
}
