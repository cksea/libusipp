/*** This Programs/Libraries are (C)opyright by Sebastian Krahmer.
 *** You may use it under the terms of the GPL. You should have
 *** already received the file COPYING that shows you your rights.
 *** Please look at COPYING for further license-details.
 ***
 *** THERE IS ABSOLUTELY NO WARRANTY. SO YOU USE IT AT YOUR OWN RISK.
 *** IT WAS WRITTEN IN THE HOPE THAT IT WILL BE USEFULL. I AM NOT RESPONSIBLE
 *** FOR ANY DAMAGE YOU MAYBE GET DUE TO USING MY PROGRAMS.
 ***/
#ifndef __datalink_h__
#define __datalink_h__

#include "usi-structs.h"
#include "RX.h"
#include <stdio.h>
#include <string>
#include <stdint.h>

extern "C" {
#include <pcap.h>
}


//#include "config.h"

namespace usipp {

/*! \class pcap
 *  \brief libpcap RX provider implementation
 */
class pcap : public RX {
private:
	struct timeval d_tv;

	// Heavily used by libpcap
	int d_datalink;
	size_t d_framelen, d_snaplen;

	// pcap-descriptor
	pcap_t *d_pd;

	// netaddress and netmask
	uint32_t d_localnet, d_netmask;

	// The actual filter-program
	struct bpf_program d_filter;

	// The pcap-header for every packet fetched
	struct pcap_pkthdr d_phdr;

	// filled by init_device()
	char d_dev[10];
	int d_has_promisc;

	// true when timed out
	bool d_timeout;

protected:
	struct ether_header d_ether;
	char d_filter_string[1000];

public:

	/*! This constructor should be used to
	 *  initialize raw-datalink-objects, means not IP/TCP/ICMP etc.
	 *  We need this b/c unlike in derived classes, datalink::init_device()
	 *  cannot set a filter!
	 */
	pcap(const std::string &);


	/*! default constructor */
	pcap();


	/*! Copy-constructor
	 */
	pcap(const pcap &);

	/*! destructor */
	virtual ~pcap();


	/*! assignment operator */
	pcap &operator=(const pcap &);


	/*! Fill buffer with src-hardware-adress of actuall packet,
	 *  use 'datalink' to determine what HW the device is.
	 *  Now only ethernet s supportet, but it's extensinable.
	 *  This is just for convenience, as rx()->get_l2src() could be used.
	 */
	virtual std::string &get_l2src(std::string &);


	/*! Fill buffer with dst-hardware-adress of actuall packet,
 	 *  use 'datalink' to determine what HW the device is.
	 *  Now only ethernet s supportet, but it's extensinable.
	 *  This is just for convenience, as rx()->get_l2src() could be used.
	 */
	virtual std::string &get_l2dst(std::string &);


	/*! Get protocol-type of ethernet-frame
	 *  Maybe moves to ethernet-class in future?
	 */
	uint16_t get_etype();


	/*! Return the actual datalink of the object.
	 */
	int get_datalink();


	/*! Return the actual framlen of the object.
	 *  (framelen depends on datalink) Not the len of the whole frame,
	*   only that of the header.
	 */
	int get_framelen();


	/*! Initialize a device ("eth0" for example) for packet-
	 *  capturing. It MUST be called before sniffpack() is launched.
	 *  Set 'promisc' to 1 if you want the device running in promiscous mode.
	 *  Fetch at most 'snaplen' bytes per call.
	 */
	virtual int init_device(const std::string &dev, int promisc, size_t snaplen);


	/*! set a new filter for capturing
	 */
	virtual int setfilter(const std::string &filter);


	/*! sniff a packet
	*/
	virtual int sniffpack(void *, size_t);


	/*! Return HW-frame (ethernet header) */
	void *get_frame(void *, size_t);


	/*! Get pcap_t struct to obtain fileno etc for select. */
	pcap_t *handle() { return d_pd; }


	/*! Set a timeout. Implements RX::timeout() = 0. */
	virtual int timeout(struct timeval);


	/*! Returns true when recv() timed out */
	virtual bool timeout();

	/*! See RX::tag() */
	virtual int tag() { return RX_TAG_PCAP; }

}; // class Datalink {}


} // namespace usipp

#endif // __datalink_h__
