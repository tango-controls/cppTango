//=============================================================================
//
// file :               DServerSignal.h
//
// description :        Include for the DServer class. This class implements
//                      all the commands which are available for device
//			of the DServer class. There is one device of the
//			DServer class for each device server process
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:41  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:15  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _DSERVERSIGNAL_H
#define _DSERVERSIGNAL_H

#include <tango.h>
#include <JTC/JTC.h>
#include <signal.h>

namespace Tango
{

//=============================================================================
//
//			The DServerSignal class
//
// description :	Class to implement all data members and commands for 
//			signal management in a TANGO device server.
//
//=============================================================================

#if ((defined sun) || defined (WIN32))
#define		_NSIG		NSIG
#endif


typedef struct
{
	vector <DeviceClass *>	registered_classes;
	vector <DeviceImpl *>	registered_devices;
}
DevSigAction;

class DServerSignal:public JTCMonitor
{
public :
	static DServerSignal *instance();
	~DServerSignal() {};
	
	void register_class_signal(long, DeviceClass *);
	void register_dev_signal(long, DeviceImpl *);
	
	void unregister_class_signal(long, DeviceClass *);
	void unregister_class_signal(DeviceClass *);
	void unregister_dev_signal(long, DeviceImpl *);
	void unregister_dev_signal(DeviceImpl *);
			
	void register_handler(long);
	void unregister_handler(long);

#ifdef __linux	
	pid_t get_sig_thread_pid();
#endif
		
	static void main_sig_handler(int);
		
	class ThSig: public JTCThread
	{
		DServerSignal *ds;
	public:
		ThSig(DServerSignal *d):ds(d) {}
		virtual ~ThSig() {}
		
		TangoSys_Pid my_pid;
#ifndef WIN32
		pthread_t my_thread;
#endif
		virtual void run();
	};
	friend class ThSig;
	ThSig *sig_th;

protected :
	DServerSignal();
	static DevSigAction		reg_sig[_NSIG];
	
	bool				sig_to_install;
	bool				sig_to_remove;
	int				inst_sig;
	int 				rem_sig;
#ifdef WIN32
	static HANDLE			win_ev;
	static int			win_signo;
#endif
		
private:
	static DServerSignal *_instance;
	vector<DeviceImpl *>::iterator find_device(long, DeviceImpl *);
	vector<DeviceImpl *>::iterator find_delayed_device(long, DeviceImpl *);
	
	vector<DeviceClass *>::iterator find_class(long, DeviceClass *);
	vector<DeviceClass *>::iterator find_delayed_class(long, DeviceClass *);

#ifdef WIN32
	static inline bool auto_signal(long s)
	{
		if ((s==SIGINT) || (s==SIGTERM) || (s==SIGABRT) || (s==SIGBREAK))
			return true;
		else
			return false;
	}	
#else
	static inline bool auto_signal(long s)
	{
		if ((s==SIGQUIT) || (s==SIGINT) || (s==SIGHUP) || (s==SIGTERM))
			return true;
		else 
			return false;
	}	
#endif

#ifdef sun
	static inline bool ign_signal(long s)
	{
		if ((s==SIGCHLD) || (s==SIGPWR) || (s==SIGWINCH) || (s==SIGURG) || (s==SIGCONT) || (s==SIGFREEZE))
			return true;
		else 
			return false;
	}
#endif

#ifdef __hpux
	static inline bool auth_signal(long s)
	{
		if ((s==SIGABRT) || (s==SIGKILL) || (s==SIGILL) ||
		    (s==SIGTRAP) || (s==SIGIOT) || (s==SIGEMT) ||
		    (s==SIGFPE) || (s==SIGBUS) || (s==SIGSEGV) ||
		    (s==SIGSYS) || (s==SIGPIPE) || (s==SIGSTOP) ||
		    (s==SIGVTALRM))
			return false;
		else
			return true;
	}
#endif
#ifdef __linux
	static inline bool auth_signal(long s)
	{
		if ((s==SIGUSR1) || (s==SIGUSR2))
			return false;
		else
			return true;
	}
#endif
#ifdef sun
	static inline bool auth_signal(long s) {return true;}
#endif
#ifdef WIN32
	static inline bool auth_signal(long s) {return true;}
#endif
	
	static string 			sig_name[_NSIG];	

};

} // End of namespace Tango


#endif
