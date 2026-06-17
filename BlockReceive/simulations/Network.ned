package blockreceive.simulations;




import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.ethernet.EthernetSwitch;
//import inet.common.misc.ThruputMeteringChannel;

network TestNetwork
{
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=300,50");
        }

        host1: StandardHost {
            @display("p=150,200");
        }

        host2: StandardHost {
            @display("p=450,200");
        }

        switch: EthernetSwitch {
            @display("p=280,201");
        }
    connections:
        host1.ethg++ <--> Eth100M <--> switch.ethg++;
        switch.ethg++ <--> Eth100M <--> host2.ethg++;
}

//
// TODO documentation
//
network TestNetwork2
{
    @display("bgb=581,115");
    submodules:
        host1: StandardHost {
            @display("p=24,51");
        }
        host2: StandardHost {
            @display("p=523,43");
        }
        router: Router {
            @display("p=269,55");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=168,18");
        }
        ethernetSwitch: EthernetSwitch {
            @display("p=400,54");
        }
        ethernetSwitch1: EthernetSwitch {
            @display("p=145,59");
        }
    connections:
        ethernetSwitch.ethg++ <--> Eth100M <--> host2.ethg++;
        host1.ethg++ <--> Eth100M <--> ethernetSwitch1.ethg++;
        ethernetSwitch1.ethg++ <--> Eth10G <--> router.ethg++;
        ethernetSwitch.ethg++ <--> Eth10G <--> router.ethg++;
}

