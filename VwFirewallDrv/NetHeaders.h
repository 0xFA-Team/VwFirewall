//	NetHeaders.h
//////////////////////////////////////////////////////////////////////////


#ifndef __NETHEADERS_HEADER__
#define __NETHEADERS_HEADER__


#pragma pack(1)

//
//	protocol
//
enum
{
	IPPROTO_IP		= 0,	//	Dummy protocol for TCP.
	IPPROTO_HOPOPTS		= 0,	//	IPv6 Hop-by-Hop options.
	IPPROTO_ICMP		= 1,	//	Internet Control Message Protocol.
	IPPROTO_IGMP		= 2,	//	Internet Group Management Protocol.
	IPPROTO_IPIP		= 4,	//	IPIP tunnels (older KA9Q tunnels use 94).
	IPPROTO_TCP		= 6,	//	Transmission Control Protocol.
	IPPROTO_EGP		= 8,	//	Exterior Gateway Protocol.
	IPPROTO_PUP		= 12,	//	PUP protocol.
	IPPROTO_UDP		= 17,	//	User Datagram Protocol.
	IPPROTO_IDP		= 22,	//	XNS IDP protocol.
	IPPROTO_TP		= 29,	//	SO Transport Protocol Class 4.
	IPPROTO_IPV6		= 41,	//	IPv6 header.
	IPPROTO_ROUTING		= 43,	//	IPv6 routing header.
	IPPROTO_FRAGMENT	= 44,	//	IPv6 fragmentation header.
	IPPROTO_RSVP		= 46,	//	Reservation Protocol.
	IPPROTO_GRE		= 47,	//	General Routing Encapsulation.
	IPPROTO_ESP		= 50,	//	encapsulating security payload.
	IPPROTO_AH		= 51,	//	authentication header.
	IPPROTO_ICMPV6		= 58,	//	ICMPv6.
	IPPROTO_NONE		= 59,	//	IPv6 no next header.
	IPPROTO_DSTOPTS		= 60,	//	IPv6 destination options.
	IPPROTO_MTP		= 92,	//	Multicast Transport Protocol.
	IPPROTO_ENCAP		= 98,	//	Encapsulation Header.
	IPPROTO_PIM		= 103,	//	Protocol Independent Multicast.
	IPPROTO_COMP		= 108,	//	Compression Header Protocol.
	IPPROTO_RAW		= 255,	//	Raw IP packets.
	IPPROTO_MAX
};

#define	IP_DF		0x4000		//	dont fragment flag
#define	IP_MF		0x2000		//	more fragments flag
#define	IP_OFFMASK	0x1fff		//	mask for fragmenting bits

//
//	TCP Flags
//
#define	TH_FIN		0x01
#define	TH_SYN		0x02
#define	TH_RST		0x04
#define	TH_PUSH		0x08
#define	TH_ACK		0x10
#define	TH_URG		0x20


//	sizeof	= 20
typedef struct tagIPHeader
{
	UCHAR	ucHeaderLength:4;	//	Header length 
	UCHAR	ucVersion:4;		//	Version
	UCHAR	ucTypeOfService;	//	Type of service
	USHORT	uTotalLength;		//	Total length
					//	���� IP ���ݰ��ĳ��ȣ����� IP ͷ���������
					//	��Ϊ�� 16bit ��������� 65535����ʵ��û����ô��
	USHORT	uId;			//	Identification
	USHORT	uOffset;		//	Fragment offset field
	UCHAR	ucTimeToLive;		//	Time to live
	UCHAR	ucProtocol;		//	Protocol
	USHORT	uChecksum;		//	Checksum
					//	IP �ײ�У��ͣ������� IP �����ײ�
	ULONG	ulSourceAddress;	//	Source address
	ULONG	ulDestinationAddress;	//	Destination address

}STIPHEADER, *LPSTIPHEADER;

typedef struct tagICMPHeader
{
	UCHAR	type;			//	Type of message 
	UCHAR	code;			//	Code
	USHORT	checksum;		//	Checksum

}STICMPHEADER, *LPSTICMPHEADER;

typedef struct tagUDPHeader
{
	USHORT	sourcePort;		//	Source port
	USHORT	destinationPort;	//	Destination port
	USHORT	length;			//	Length
	USHORT	checksum;		//	Checksum

}STUDPHEADER, *LPSTUDPHEADER;

typedef struct tagTCPHeader
{
	USHORT	sourcePort;		//	16 λԴ�˿�(Source Port)
	USHORT	destinationPort;	//	16 λĿ�Ķ˿�(Destination Port)
	ULONG	nSequence;		//	32 λ���к�(Sequence number)
	ULONG	nAck;			//	32 λȷ�Ϻ�(Acknowledgement number)


	UCHAR	unused:4;		//	6 λ������(Reserved for future use. Must be zero.)

	UCHAR	offset:4;		//	Data offset
					//	The number of 32 bit words in the TCP Header.  This indicates where
					//	the data begins.  The TCP header (even one including options) is an
					//	integral number of 32 bits long.
					//	TCP���ݿ�ʼλ�� = offset * 4;

	UCHAR	flags;			//	6 λ��־λ�������ã�(Flags)
					//	URG:  Urgent Pointer field significant
					//	ACK:  Acknowledgment field significant
					//	PSH:  Push Function
					//	RST:  Reset the connection
					//	SYN:  Synchronize sequence numbers
					//	FIN:  No more data from sender


	USHORT	window;			//	16 λ���ڴ�С(Window size)
	USHORT	checksum;		//	16 λ TCP У���(Checksum)
	USHORT	urp;			//	16 λ��������ƫ����(Urgent Pointer)

	//u_short hlen_sv_control;//HLEN(4 bit)+����(6 bit)+�����ֶ�(6 bit,URG+ACK+PSH+PST+SYN+FIN) 
	
	//Options: no options included here (we assume that the MSS (max.
	//segment size) has the default value of 536 bytes)

	//Padding: not included here

}STTCPHEADER, *LPSTTCPHEADER;

//
//	TCPα�ײ� ���ڽ���TCPУ��͵ļ���,��֤TCPЧ�����Ч��
//	sizeof	= 12
//
typedef struct tagTCPPsdHeader
{
	ULONG sourceip;		//	ԴIP��ַ
	ULONG destip;		//	Ŀ��IP��ַ
	UCHAR mbz;		//	�ÿ�(0)
	UCHAR ptcl;		//	Э������(IPPROTO_TCP)
	USHORT tcpl;		//	TCP�����ܳ���(��λ:�ֽ�)

}STTCPPSDHEADER, *LPSTTCPPSDHEADER;




#pragma pack()




#endif
