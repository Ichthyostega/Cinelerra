#ifndef LOADBALANCE_H
#define LOADBALANCE_H

#include "mutex.h"
#include "thread.h"




// Load balancing utils



class LoadServer;



class LoadPackage
{
public:
	LoadPackage();
	virtual ~LoadPackage();
	
	Mutex completion_lock;
};


class LoadClient : public Thread
{
public:
	LoadClient(LoadServer *server);
	virtual ~LoadClient();
	
	void run();
	virtual void process_package(LoadPackage *package) {};
	int get_package_number();

	int done;
	int package_number;
	Mutex input_lock, completion_lock;
	LoadServer *server;
};




class LoadServer
{
public:
	LoadServer(int total_clients, int total_packages);
	virtual ~LoadServer();

	friend class LoadClient;

	virtual void init_packages() {};
	virtual LoadClient* new_client() { return 0; };
	virtual LoadPackage* new_package() { return 0; };

	void process_packages();
	int get_total_packages();
	int get_total_clients();
	LoadPackage* get_package(int number);
	LoadClient* get_client(int number);
	void set_package_count(int total_packages);



	void delete_clients();
	void create_clients();
	void delete_packages();
	void create_packages();





	int current_package;
	LoadPackage **packages;
	int total_packages;
	LoadClient **clients;
	int total_clients;
	Mutex client_lock;
};



#endif

