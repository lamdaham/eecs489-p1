#!/usr/bin/env python

from mininet.net import Mininet
from mininet.topo import Topo
from mininet.link import TCLink
from mininet.log import setLogLevel
from mininet.cli import CLI

class CustomFiveFiveTopo(Topo):
    def __init__(self, **opts):
        # Initialize topology
        super(CustomFiveFiveTopo, self).__init__(**opts)

        # Add hosts
        h1 = self.addHost('h1')
        h2 = self.addHost('h2')
        h3 = self.addHost('h3')
        h4 = self.addHost('h4')
        h5 = self.addHost('h5')

        # Add switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')
        s5 = self.addSwitch('s5')  

        # Create host-switch links (with default link parameters)
        self.addLink(h1, s2)
        self.addLink(h2, s2)
        self.addLink(h3, s3)
        self.addLink(h4, s3)
        self.addLink(h5, s4)

        # Create switch-switch links with bandwidth and delay constraints
        # Feel free to adjust these values or add new links for exploration
        self.addLink(s2, s1)
        self.addLink(s1, s3)
        self.addLink(s1, s4)
        self.addLink(s4, s5)

if __name__ == '__main__':
    setLogLevel( 'info' )

    # Create data network
    topo = CustomFiveFiveTopo()
    net = Mininet(topo=topo, link=TCLink, autoSetMacs=True,
           autoStaticArp=True)

    # Run network
    net.start()
    CLI( net )
    net.stop()