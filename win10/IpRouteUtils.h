#ifndef IPROUTEUTILS_H
#define IPROUTEUTILS_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <WS2tcpip.h>
#include<Iphlpapi.h>
#include<rtmv2.h>
#include "nldef.h"

#include <QDebug>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")

#define PROTO_TYPE_UCAST     0
#define PROTOCOL_ID(Type, VendorId, ProtocolId) (((Type & 0x03)<<30)|((VendorId & 0x3FFF)<<16)|(ProtocolId & 0xFFFF))
#define PROTO_VENDOR_ID      0xFFFF

#define WORKING_BUFFER_SIZE 81920
#define MAX_TRIES 3

#define ERR_NO_RESULT					1
#define ERR_ALLOC						2		//HeapAlloc IP_ADAPTER_ADDRESSES
#define ERR_GET_ADAPTERS_ADDRESSES      3		//GetAdaptersAddresses ERROR
#define SUCCESS							0	
#define ERRCREATEIPREROUT				4
#define ERRDELETEIPREROUT				5

typedef struct VpnIpAddr
{
    ULONG IfIndex;						//vpn设备的Ifindex
    IN_ADDR* Addr;						//Vpn设备的ip IN_ADDR格式
    const char FAR * ForwardDest;		//目标主机的IP地址
    const char FAR * ForwardMask;		//目标主机的子网掩码
    const char FAR * ForwardNextHop;	//路由中下一跳的IP地址
    const char FAR * VpnServer;	
    CHAR IP[130];                      // ip in string format
} VpnIpAddrS;

/*
    VpnIpAddrS* vid =  new VpnIpAddrS();
    vid->ForwardDest = "10.0.0.0";
    vid->ForwardMask = "255.255.255.0";
    vid->ForwardNextHop = "0.0.0.0";
    vid->VpnServer = "36.110.115.116";
    
    IpRouteUtils ipu;
    //获取vpn设备的ip和IfIndex
    ipu.GetIpAddr(L"一号设备", vid);
    ipu.CreateIpRoute(vid);
    ipu.DeleteIpRoute(vid);
*/
class IpRouteUtils
{
public :
    IpRouteUtils();

    void getAdapterAddr();

    DWORD GetIpAddr(LPCWCHAR adapterName, VpnIpAddrS* vip);
    DWORD CreateIpRoute(VpnIpAddrS *vip);
    DWORD DeleteIpRoute(VpnIpAddrS *vip);
    //DWORD CreateIpRouteUseRtm(VpnIpAddrS *vip);
};

#endif //IPROUTEUTILS_H
