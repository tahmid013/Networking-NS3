from xml.etree import ElementTree as ET 
import sys
et=ET.parse(sys.argv[1])

node=int(sys.argv[2])


f1=open("throughput_t.txt","a")
f2=open("endToEndDelay.txt","a")
f3=open("packetDeliveryRatio.txt","a")
f4=open("packetDropRatio.txt","a")
f5=open("fairness.txt","a")


total_rx_byte = 0
total_rx_packets = 0
total_tx_packets = 0
total_delay = 0.0
simulation_start = 0.0
simulation_stop = 0.0
total_thr = 0.0
total_sq_thr = 0.0
flowCount = 0
totalTime=0.0


for flow in et.findall("FlowStats/Flow"):
    receivedByte = int(flow.get('rxBytes'))
    receivedPacket = int(flow.get('rxPackets'))
    transmittedPacket = int(flow.get('txPackets'))
    
    end_to_end_delay = float(flow.get('delaySum')[:-2])


    
    flowCount+=1
    total_rx_byte+= receivedByte
    total_rx_packets += receivedPacket
    total_tx_packets += transmittedPacket
    total_delay += end_to_end_delay
    
    tx_start = float(flow.get('timeFirstRxPacket')[:-2])
    tx_stop = float(flow.get('timeLastRxPacket')[:-2])
  

    if simulation_start == 0.0:
       simulation_start = tx_start

   
    simulation_start = min(tx_start, simulation_start)
    simulation_stop = max(tx_stop, simulation_stop)

        
    time = simulation_stop - simulation_start
    
    throughput=0
    if time!=0:
        throughput = ((receivedByte * 8.0)/time)/1024
    total_thr += throughput
    total_sq_thr += throughput ** 2
        
    deliveryRatio = receivedPacket/transmittedPacket
    dropRatio = (transmittedPacket-receivedPacket)/transmittedPacket    
        
    print("Packet transmitted in flow-"+flow.get('flowId')+": "+str(transmittedPacket))
    print("Packet received in flow-"+flow.get('flowId')+": "+str(receivedPacket))
    print("Packet Delivery ratio- "+str(deliveryRatio))
    print("Packet Drop ratio- "+str(dropRatio))
    print("End to end Delay- "+str(end_to_end_delay)+"sec\n")
   
    

print("\n-------Througput------\n")

print("Total Received bits = "+str(total_rx_byte*8))

duration = (simulation_stop-simulation_start)*1e-9
print("Receiving Packet Duration time = %.3f sec" % (duration,))
print("Network Throughput = %.3f Mbps" % (total_rx_byte*8/(duration*1e6),))

f1.write(str(node)+" "+str(total_rx_byte*8/(duration*1e6))+"\n")



print("\n------Delivery Ratio------- \n")

print("Total Transmitted Packets = "+str(total_tx_packets))
print("Total Received Packets = "+str(total_rx_packets))
total_dropped_packets=total_tx_packets-total_rx_packets

print("Packet Delivery Ratio = %.3f " % (total_rx_packets*1.0/total_tx_packets,))
f3.write(str(node)+" "+str(total_rx_packets*1.0/total_tx_packets)+"\n")


print("\n------Drop Ratio------- \n")

print("Packet Drop Ratio = %.3f " % (total_dropped_packets*1.0/total_tx_packets,))
f4.write(str(node)+" "+str(total_dropped_packets*1.0/total_tx_packets)+"\n")

print("\n------Delay------- \n")
print("Network Delay = %.3f ms" % (total_delay*1e-6/total_rx_packets,))
f2.write(str(node)+" "+str(total_delay*1e-6/total_rx_packets)+"\n")

print("\nFairness Calculation : ")
FairnessIndex = (total_thr * total_thr)/ (flowCount * total_sq_thr) ;
print("Jain's Index: "+str(FairnessIndex))
f5.write(str(node)+" "+str(FairnessIndex)+"\n")




