#include "vxWorks.h"
#include "sys/sysctl.h"
#include "stdio.h"
#include <./ipcom/include/ipcom_type.h>
#include <./ipnet2/include/ipnet_routesock.h>
#include <./ipcom/include/ipcom_sysctl.h>
#include <./ipnet2/src/ipnet_cmd.h>
#include <./ipcom/include/ipcom_clib.h>
#include <./ipcom/include/ipcom_clib.h>
#include <./ipcom/include/ipcom_sock.h>


#define IP_IFT_L2VLAN      0x87   /* Layer 2 Virtual LAN using 802.1Q */

int vlan_check(int ifindex, char * parent)
{
    struct Ip_ifreq   ifreq;
    struct Ip_vlanreq vlanreq;
    
    int fd;
        fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
        
    ipcom_memset(&vlanreq, 0, sizeof(struct Ip_vlanreq));
    ifreq.ip_ifr_data = &vlanreq;
    if (ipcom_if_indextoname(ifindex, ifreq.ifr_name) == IP_NULL
        || ipcom_socketioctl(fd, IP_SIOCGETVLAN, &ifreq) < 0)
        return 0;

    if(ipcom_strcmp(parent,vlanreq.vlr_parent) == 0)
    { 
        	return 1;
        }
    else
    	return -1; 
    
}



int get_ip( char * ifname)
{ 
	struct Ip_ifreq ifr;
	int fd;
	int addr;
	
	        fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
	
	ipcom_memset(&ifr,0,sizeof(ifr));
	ipcom_strcpy(ifr.ifr_name, ifname); 
	if (ipcom_socketioctl(fd, IP_SIOCGIFADDR, &ifr) != IP_SOCKERR)
	{ 
		addr =((struct Ip_sockaddr_in *)&ifr.ip_ifr_addr)->sin_addr.s_addr;
		
	return addr;
	}
}

void test_if(char *parent)
{
	
	int                     name[6];
	Ip_u8                  *if_data;
	Ip_size_t               if_data_len;
	Ip_size_t               if_data_offset;
	struct Ipnet_if_msghdr *ifm;
	struct Ip_sockaddr     *hwaddr;
	char                    ifname[IP_IFNAMSIZ];
	char                    ifname2[IP_IFNAMSIZ];
	char                    str[80];

    
	name[0] = IP_CTL_NET;
	name[1] = IP_AF_ROUTE;
	name[2] = 0;    /* Always 0 */
	name[3] = 0;
	name[4] = IP_NET_RT_IFLIST;
	name[5] = 0;
	
	 if (ipcom_sysctl(name, 6, IP_NULL, &if_data_len, IP_NULL, 0) < 0)
	 {
		 printf("ifconfig: ipnet_sysctl() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
		 return;
	 }

	 if_data = ipcom_malloc(if_data_len);
	 if (if_data == IP_NULL)
	 {
		 printf("ifconfig: out of memory"IP_LF);
		 return;
	 }

	 if (ipcom_sysctl(name, 6, if_data, &if_data_len, IP_NULL, 0) < 0)
	 {
		 if (ipcom_errno == IP_ERRNO_ENOMEM)
		 {
			 ipcom_free(if_data);
		 }
		 printf("ifconfig: ipnet_sysctl() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
	 }

	 if_data_offset = 0;
	 while (if_data_len > if_data_offset + sizeof(struct Ipnet_if_msghdr))
	 {
		 ifm = (struct Ipnet_if_msghdr *) &if_data[if_data_offset];
		 if_data_offset += ifm->ifm_msglen;

		 /* Resolve the interface name */
		 		 if (ipcom_if_indextoname(ifm->ifm_index, ifname) == IP_NULL)
		 			 continue;	

		 		if (ifm->ifm_data.ifi_type == IP_IFT_L2VLAN)
		 		{
		 			
		 			if (( vlan_check(ifm->ifm_index, parent) == 0) || (vlan_check(ifm->ifm_index, parent) == -1))
		 				continue;
		 			else
		 			{
		 				printf("Interface=%s\n", ifname);
		 				ipcom_if_indextoname(ifm->ifm_index, ifname2);
		 				return get_ip(ifname2);
		 				
		 			}
		 		}	
		 		 		
   }		
}							

