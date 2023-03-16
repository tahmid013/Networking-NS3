/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
// #include <fstream>
// #include "ns3/core-module.h"
// #include "ns3/point-to-point-module.h"
// #include "ns3/network-module.h"
// #include "ns3/applications-module.h"
// #include "ns3/mobility-module.h"
// #include "ns3/csma-module.h"
// #include "ns3/internet-module.h"
// #include "ns3/yans-wifi-helper.h"
// #include "ns3/ssid.h"
 #include "ns3/netanim-module.h"
// #include "ns3/stats-module.h"
// #include "ns3/netanim-module.h"
// #include "ns3/packet-sink-helper.h"
// #include "ns3/packet-sink.h"
// #include "ns3/flow-monitor-helper.h"

#include <fstream>

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"

// Task A1 ( Wired )
//                      sink-> (right)[nCsma-2]
//                      src-> (left)[nCsma-1]
                
//                         10.1.1.0
// .... n5   n6   n7  n0 -------------- n1   n2   n3   n4 ....
//      |    |     |   |     p-to-p      |    |    |    |
//      ===============                ================
//        LAN 10.1.3.0                    LAN 10.1.2.0

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");
class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  /**
   * Register this type.
   * \return The TypeId.
   */
  static TypeId GetTypeId (void);
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

/* static */
TypeId MyApp::GetTypeId (void)
{
  static TypeId tid = TypeId ("MyApp")
    .SetParent<Application> ()
    .SetGroupName ("Tutorial")
    .AddConstructor<MyApp> ()
    ;
  return tid;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  if (InetSocketAddress::IsMatchingType (m_peer))
    {
      m_socket->Bind ();
    }
  else
    {
      m_socket->Bind6 ();
    }
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}


std::ofstream throughput("throughput.txt");
Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */

void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  //std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  throughput << now.GetSeconds () <<" "<< cur << std::endl;
  lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsmar = 50;
  uint32_t nCsmal = 5;
  int flow =8;
  int pps = 100;
  int payload = 100;

  bool tracing = false;

  CommandLine cmd (__FILE__);
  
  //cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.AddValue ("nCsmar", "Csma nodes in right", nCsmar);
  cmd.AddValue ("nCsmal", "Csma nodes in left", nCsmal);
  cmd.AddValue ("flow", "number of Flow ", flow);
  cmd.AddValue ("pps", "number of datarate ", pps);

  cmd.Parse (argc,argv);

  flow = flow/2;

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer csmaNodesR;
  csmaNodesR.Add (p2pNodes.Get (1));
  csmaNodesR.Create (nCsmar);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevicesR;
  csmaDevicesR = csma.Install (csmaNodesR);


  NodeContainer csmaNodesL;
  csmaNodesL.Add (p2pNodes.Get (0));
  csmaNodesL.Create (nCsmal);

  CsmaHelper csmaL;
  csmaL.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csmaL.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevicesL;
  csmaDevicesL = csmaL.Install (csmaNodesL);


  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.0001));
  //csmaDevices.Get (nCsma-1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (csmaNodesR);
  stack.Install (csmaNodesL);


  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfacesR;
  csmaInterfacesR = address.Assign (csmaDevicesR);
  
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfacesL;
  csmaInterfacesL = address.
  Assign (csmaDevicesL);

  std::string probeType;
  std::string tracePath;

  probeType = "ns3::Ipv4PacketProbe";
  tracePath = "/NodeList/0/$ns3::Ipv4L3Protocol/Tx";

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();




  
    



uint16_t sinkPort = 1080;
Address sinkAddress;
Address anyAddress;

Ptr <Socket> socket;
std::string dataRate = std::to_string((8*pps*payload) / 1e6) + "Mbps";

  
for(int i=0;i<flow;i++)
  {
    csmaDevicesR.Get(i%nCsmar)->SetAttribute ("ReceiveErrorModel", PointerValue (em));      
    csmaDevicesL.Get(i%nCsmal)->SetAttribute ("ReceiveErrorModel", PointerValue (em));      
    p2pDevices.Get(i%2)->SetAttribute ("ReceiveErrorModel", PointerValue (em));      

    sinkAddress = InetSocketAddress (csmaInterfacesR.GetAddress (i%nCsmar), sinkPort);
    anyAddress = InetSocketAddress (Ipv4Address::GetAny (), sinkPort);


    PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", anyAddress);
    ApplicationContainer sinkApps = packetSinkHelper.Install (csmaNodesR.Get (i));
    sink=StaticCast<PacketSink>(sinkApps.Get(0));
    sinkApps.Start (Seconds (0.));
    sinkApps.Stop (Seconds (3.));

    socket = Socket::CreateSocket (csmaNodesL.Get (i%nCsmal), TcpSocketFactory::GetTypeId ());
    //socket->TraceConnectWithoutContext ("MyInteger", MakeCallback(&CalculateThroughput));


    Ptr<MyApp> app = CreateObject<MyApp> ();
    app->Setup (socket, sinkAddress, 1040, 1000, dataRate);
    csmaNodesL.Get (i%nCsmal)->AddApplication (app);
    app->SetStartTime (Seconds (1.));
    app->SetStopTime (Seconds (3.));
    
    sinkPort++;

  }


  


Simulator::Stop (Seconds (20.0));




  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor=flowHelper.InstallAll();
  flowMonitor->CheckForLostPackets ();


  
  //Simulator::Stop (Seconds (4));
  Simulator::Run ();

  flowMonitor->SerializeToXmlFile("./task_A_wired.flowmonitor",false,false);
  double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * 4));

  std::cout << "Average Throughput: "<<averageThroughput<<"Mbit/s" <<std::endl;

  Simulator::Destroy ();

  return 0;


  
 
}
