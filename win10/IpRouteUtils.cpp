#include "IpRouteUtils.h"
#include <iostream>

IpRouteUtils::IpRouteUtils(){

}

void IpRouteUtils::getAdapterAddr()
{
    DWORD dwRetVal = 0;
    PIP_ADAPTER_ADDRESSES pAddresses  = NULL;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    ULONG outBufLen = WORKING_BUFFER_SIZE;
    ULONG Iterations = 0;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {

        pAddresses  = (IP_ADAPTER_ADDRESSES *) HeapAlloc(GetProcessHeap(), 0, outBufLen);
        if (pAddresses  == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            exit(1);
        }

        dwRetVal = GetAdaptersAddresses(
                    AF_UNSPEC, //Return both IPv4 and IPv6 addresses
                    0,
                    NULL,  //reserved,
                    pAddresses , //A pointer to a buffer
                    &outBufLen
                    );

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            HeapFree(GetProcessHeap(), 0, pAddresses );
            pAddresses  = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

//    if (dwRetVal == NO_ERROR) {
//            // If successful, output some information from the data we received
//            pCurrAddresses = pAddresses;
//            while (pCurrAddresses) {
//                printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n",
//                       pCurrAddresses->Length);
//                printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex);
//                printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

//                pUnicast = pCurrAddresses->FirstUnicastAddress;
//                if (pUnicast != NULL) {
//                    for (i = 0; pUnicast != NULL; i++)
//                        pUnicast = pUnicast->Next;
//                    printf("\tNumber of Unicast Addresses: %d\n", i);
//                } else
//                    printf("\tNo Unicast Addresses\n");

//                pAnycast = pCurrAddresses->FirstAnycastAddress;
//                if (pAnycast) {
//                    for (i = 0; pAnycast != NULL; i++)
//                        pAnycast = pAnycast->Next;
//                    printf("\tNumber of Anycast Addresses: %d\n", i);
//                } else
//                    printf("\tNo Anycast Addresses\n");

//                pMulticast = pCurrAddresses->FirstMulticastAddress;
//                if (pMulticast) {
//                    for (i = 0; pMulticast != NULL; i++)
//                        pMulticast = pMulticast->Next;
//                    printf("\tNumber of Multicast Addresses: %d\n", i);
//                } else
//                    printf("\tNo Multicast Addresses\n");

//                pDnServer = pCurrAddresses->FirstDnsServerAddress;
//                if (pDnServer) {
//                    for (i = 0; pDnServer != NULL; i++)
//                        pDnServer = pDnServer->Next;
//                    printf("\tNumber of DNS Server Addresses: %d\n", i);
//                } else
//                    printf("\tNo DNS Server Addresses\n");

//                printf("\tDNS Suffix: %wS\n", pCurrAddresses->DnsSuffix);
//                printf("\tDescription: %wS\n", pCurrAddresses->Description);
//                printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);

//                if (pCurrAddresses->PhysicalAddressLength != 0) {
//                    printf("\tPhysical address: ");
//                    for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength;
//                         i++) {
//                        if (i == (pCurrAddresses->PhysicalAddressLength - 1))
//                            printf("%.2X\n",
//                                   (int) pCurrAddresses->PhysicalAddress[i]);
//                        else
//                            printf("%.2X-",
//                                   (int) pCurrAddresses->PhysicalAddress[i]);
//                    }
//                }
//                printf("\tFlags: %ld\n", pCurrAddresses->Flags);
//                printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
//                printf("\tIfType: %ld\n", pCurrAddresses->IfType);
//                printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
//                printf("\tIpv6IfIndex (IPv6 interface): %u\n",
//                       pCurrAddresses->Ipv6IfIndex);
//                printf("\tZoneIndices (hex): ");
//                for (i = 0; i < 16; i++)
//                    printf("%lx ", pCurrAddresses->ZoneIndices[i]);
//                printf("\n");

//                printf("\tTransmit link speed: %I64u\n", pCurrAddresses->TransmitLinkSpeed);
//                printf("\tReceive link speed: %I64u\n", pCurrAddresses->ReceiveLinkSpeed);

//                pPrefix = pCurrAddresses->FirstPrefix;
//                if (pPrefix) {
//                    for (i = 0; pPrefix != NULL; i++)
//                        pPrefix = pPrefix->Next;
//                    printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
//                } else
//                    printf("\tNumber of IP Adapter Prefix entries: 0\n");

//                printf("\n");

//                pCurrAddresses = pCurrAddresses->Next;
//            }


}

DWORD IpRouteUtils::GetIpAddr(LPCWCHAR adapterName, VpnIpAddrS* vip)
{
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;//包括 IPV4 ，IPV6 网关
    ULONG family = AF_UNSPEC;//返回包括 IPV4 和 IPV6 地址
    IP_ADAPTER_ADDRESSES * pCurrAddresses = nullptr;

    // specifies the size of the buffer pointed to by AdapterAddresses.
    ULONG sizeOfAdapterAddresses = WORKING_BUFFER_SIZE;

    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;

    IP_ADAPTER_ADDRESSES *pAdapterAddresses = (IP_ADAPTER_ADDRESSES *)HeapAlloc(GetProcessHeap(), 0, sizeOfAdapterAddresses);
    if (pAdapterAddresses == NULL)
    {
        return ERR_ALLOC;
    }

    ULONG dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAdapterAddresses, &sizeOfAdapterAddresses);

    if (dwRetVal == NO_ERROR)
    {
        pCurrAddresses = pAdapterAddresses;
        while (pCurrAddresses)
        {
            PWCHAR fn = pCurrAddresses->FriendlyName;
            if (_wcsicmp(adapterName, pCurrAddresses->FriendlyName) == 0) {
                vip->IfIndex = pCurrAddresses->IfIndex;

                pUnicast = pCurrAddresses->FirstUnicastAddress;
                while (pUnicast)//单播IP
                {
                    if (AF_INET == pUnicast->Address.lpSockaddr->sa_family)// IPV4 地址，使用 IPV4 转换
                    {
                        vip->Addr = (&((sockaddr_in*)pUnicast->Address.lpSockaddr)->sin_addr);

                        //converts an IPv4 or IPv6 Internet network address into a string in Internet standard format
                        inet_ntop(AF_INET, vip->Addr, vip->IP, sizeof(vip->IP));
                    }
                    pUnicast = pUnicast->Next;
                }
                pUnicast = NULL;
                pCurrAddresses = NULL;

                goto END;
            }
            pCurrAddresses = pCurrAddresses->Next;
        }
        pCurrAddresses = NULL;
        return ERR_NO_RESULT;
    }
    else {
        if (pAdapterAddresses)
            HeapFree(GetProcessHeap(), 0, pAdapterAddresses);
        return ERR_GET_ADAPTERS_ADDRESSES;
    }

END:
    if (pAdapterAddresses)
        HeapFree(GetProcessHeap(), 0, pAdapterAddresses);
    return SUCCESS;
}

DWORD IpRouteUtils::CreateIpRoute(VpnIpAddrS *vip) {
    ULONG idx = 0;
    INT addr = 0;
    inet_pton(PF_INET, vip->VpnServer, &addr);
    if (NO_ERROR != GetBestInterface(addr, &idx))
    {
        return ERRCREATEIPREROUT;
    }

    MIB_IPINTERFACE_ROW aRow ;
    aRow.Family = AF_INET;
    aRow.InterfaceLuid = { 0 };
    aRow.InterfaceIndex = idx;

    if (GetIpInterfaceEntry(&aRow) != NO_ERROR) {
        return ERRCREATEIPREROUT;
    }

    MIB_IPFORWARDROW ipForwardRow;

    //destination IPv4 address of the route
    inet_pton(PF_INET, vip->ForwardDest, &(ipForwardRow.dwForwardDest));

    //IPv4 subnet mask to use with the destination IPv4 address
    inet_pton(PF_INET, vip->ForwardMask, &(ipForwardRow.dwForwardMask));

    //for remote route, the IPv4 address of the next system en route.
    inet_pton(PF_INET, vip->ForwardNextHop, &(ipForwardRow.dwForwardNextHop));

    //The index of the local interface through which the next hop of this route should be reached
    ipForwardRow.dwForwardIfIndex = vip->IfIndex;


    ipForwardRow.dwForwardProto = MIB_IPPROTO_NETMGMT;

    //The route type. The remote route where the next hop is not the final destination (a remote destination
    ipForwardRow.dwForwardType = MIB_IPROUTE_TYPE_INDIRECT;

    ipForwardRow.dwForwardMetric1 = aRow.Metric;

    //not used by ipv4
    //ipForwardRow.dwForwardPolicy = 0;

    DWORD dwStatus = CreateIpForwardEntry(&ipForwardRow);

    switch (dwStatus) {

    case NO_ERROR:
        printf("Gateway changed successfully\n");
        return SUCCESS;
    case ERROR_INVALID_PARAMETER:
        printf("Invalid parameter.\n");
        qDebug() << "Invalid parameter";
        return ERRCREATEIPREROUT;
    case ERROR_ACCESS_DENIED:
        qDebug() << "access denied";
        return ERRCREATEIPREROUT;
    default:
        qDebug() << "Error: " << dwStatus;
        printf("Error: %d\n", dwStatus);
        return ERRCREATEIPREROUT;
    }
}

DWORD IpRouteUtils::DeleteIpRoute(VpnIpAddrS *vip) {
    MIB_IPFORWARDROW IpForwardTable;
    inet_pton(PF_INET, vip->ForwardDest, &(IpForwardTable.dwForwardDest));
    inet_pton(PF_INET, vip->ForwardMask, &(IpForwardTable.dwForwardMask));
    inet_pton(PF_INET, vip->ForwardNextHop, &(IpForwardTable.dwForwardNextHop));
    IpForwardTable.dwForwardIfIndex = vip->IfIndex;
    IpForwardTable.dwForwardProto = MIB_IPPROTO_NETMGMT;
    IpForwardTable.dwForwardType = MIB_IPROUTE_TYPE_INDIRECT;
    IpForwardTable.dwForwardMetric1 = -1;
    IpForwardTable.dwForwardPolicy = 0;

    DWORD dwRetVal = DeleteIpForwardEntry(&IpForwardTable);
    if (NO_ERROR == dwRetVal)
    {
        return SUCCESS;
    }
    else
    {
        return ERRDELETEIPREROUT;
    }
}

//DWORD IpRouteUtils::CreateIpRouteUseRtm(VpnIpAddrS *vip) {
//	RTM_ENTITY_HANDLE RtmRegHandle;
//	RTM_ENTITY_INFO EntityInfo;
//	RTM_REGN_PROFILE RegnProfile;
//	DWORD dwRet = ERROR_SUCCESS;
//
//	EntityInfo.RtmInstanceId = 0;
//	EntityInfo.AddressFamily = AF_INET;
//	EntityInfo.EntityId.EntityProtocolId = PROTO_IP_RIP;
//	EntityInfo.EntityId.EntityInstanceId = PROTOCOL_ID(PROTO_TYPE_UCAST, PROTO_VENDOR_ID, PROTO_IP_RIP);
//
//	// Register the new entity
//	dwRet = RtmRegisterEntity(&EntityInfo, NULL, NULL, FALSE, &RegnProfile, &RtmRegHandle);
//	if (dwRet != ERROR_SUCCESS) {
//		// Registration failed - Log an Error and Quit
//	}
//
//	//// Clean-up: Deregister the new entity
//	//dwRet = RtmDeregisterEntity(RtmRegHandle);
//	//if (dwRet != ERROR_SUCCESS) {
//	//	// Registration failed - Log an Error and Quit
//	//}
//	// Add a route to a destination given by (addr, masklen)
//	// using a next hop reachable with an interface
//
//	RTM_NEXTHOP_INFO NextHopInfo;
//	HANDLE NextHopHandle;
//	// First, create and add a next hop to the caller's
//	// next-hop tree (if it does not already exist)
//
//	ZeroMemory(&NextHopInfo, sizeof(RTM_NEXTHOP_INFO));
//
//	RTM_IPV4_MAKE_NET_ADDRESS(&NextHopInfo.NextHopAddress,
//		inet_addr(vip->ForwardNextHop), // Address of the next hop
//		32);
//
//	NextHopInfo.InterfaceIndex = vip->IfIndex;
//
//	NextHopHandle = NULL;
//	DWORD ChangeFlags=0;
//
//	DWORD Status = RtmAddNextHop(RtmRegHandle,
//		&NextHopInfo,
//		&NextHopHandle,
//		&ChangeFlags);
//
//	if (Status == NO_ERROR)
//	{
//		// Created a new next hop or found an old one
//		RTM_ROUTE_INFO RouteInfo;
//		HANDLE RouteHandle;
//		HANDLE RouteListHandle1;
//			// Fill in the route information for the route
//
//		ZeroMemory(&RouteInfo, sizeof(RTM_ROUTE_INFO));
//
//		// Fill in the destination network's address and mask values
//		RTM_IPV4_MAKE_NET_ADDRESS(&NetAddress, inet_addr(vip->ForwardDest), 32);
//
//		// Assume 'neighbour learnt from' is the first next hop
//		RouteInfo.Neighbour = NextHopHandle;
//
//		// Set metric for route; Preference set internally
//		RouteInfo.PrefInfo.Metric = 1;
//
//		// Adding a route to both the unicast and multicast views
//		RouteInfo.BelongsToViews = RTM_VIEW_MASK_UCAST | RTM_VIEW_MASK_MCAST;
//
//		RouteInfo.NextHopsList.NumNextHops = 1;
//		RouteInfo.NextHopsList.NextHops[0] = NextHopHandle;
//
//		// If you want to add a new route, regardless of
//		// whether a similar route already exists, use the following 
//		//     ChangeFlags = RTM_ROUTE_CHANGE_NEW;
//
//		ChangeFlags = 0;
//
//		Status = RtmAddRouteToDest(RtmRegHandle,
//			&RouteHandle,     // Can be NULL if you do not need handle
//			&NetAddress,
//			&RouteInfo,
//			1000,             // Time out route after 1000 ms
//			RouteListHandle1, // Also add the route to this list
//			0,
//			NULL,
//			&ChangeFlags);
//
//		if (Status == NO_ERROR)
//		{
//			if (ChangeFlags & RTM_ROUTE_CHANGE_NEW)
//			{
//				; // A new route has been created
//			}
//			else
//			{
//				; // An existing route is updated
//			}
//
//			if (ChangeFlags & RTM_ROUTE_CHANGE_BEST)
//			{
//				; // Best route information has changed
//			}
//
//			// Release the route handle if you do not need it
//			RtmReleaseRoutes(RtmRegHandle, 1, &RouteHandle);
//		}
//
//		// Also release the next hop since it is no longer needed 
//		RtmReleaseNextHops(RtmRegHandle, 1, &NextHopHandle);
//	}
//	return 0;
//}
