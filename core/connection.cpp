/**
 *  Connection.cpp
 * 
 *  Class representing a connection to a MySQL or MariaDB daemon
 * 
 * 	@copyright 2014 Copernica BV
 */

/**
 *  Dependencies
 */
#include "connection.h"
#include "loop.h"
#include "result.h"
#include "localparameter.h"
#include <iostream>
#include <cassert>
#include <phpcpp.h>

/**
 *	Set up namespace
 */
namespace ReactPhp {
	
/**
 *	Establish a connection to mysql
 *
 *	@param loop 	the loop to bind to
 *	@param hostname the hostname to connect to
 *	@param username the username to login with
 *	@param password the password to authenticate with
 *	@param database the database to use
 *	@param callback the callback to inform once the connection is established or failed
 */
void Connection::__construct(Php::Parameters &params)
{
	// retrieve the parameters
	Php::Value loopParam = params[0];
	Php::Value hostname = params[1];
	Php::Value username = params[2];
	Php::Value password = params[3];
	Php::Value database = params[4];
	Php::Value callback = params[5];
		
	// get the actual base objects
	Loop *loop = (Loop *)loopParam.implementation();
		
	// create the actual connection
	_connection = std::make_shared<React::MySQL::Connection>(loop->loop(), hostname, username, password, database, [callback](React::MySQL::Connection *connection, const char *error) {
		
		if (error) throw Php::Exception(error); 
		
		// call the PHP callback
		callback();
	});
}	
	  
/**
 *	Execute a query
 *
 *	@param query	the query to execute
 *	@param callback	the callback to inform for all the result sets generated by the query
 */
void Connection::query(Php::Parameters &parameters)
{
	// retrieve the parameters
	Php::Value query = parameters[0];
	Php::Value callback = parameters[1];
	
	// perform the actual query	
	_connection->query(query, [callback](React::MySQL::Result&& result, const char *error) {
		
		// check if there is a query error
		if (error) throw Php::Exception(error);
		
		// wrap the result in a PHP object
		Result *object = new Result(std::move(result));

		// call the PHP callback
		callback(Php::Object("Async\\Result", object));
		
		// stop the application
		exit(0);
	});	
}

/**
 *  End namespace
 */
}
