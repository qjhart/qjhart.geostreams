/* Copyright (C) 1999 Lucent Technologies */
/* Excerpted from 'The Practice of Programming' */
/* by Brian W. Kernighan and Rob Pike */
// updated by carueda 2004-09-16
// Startpan_Id: Csv.h,v 1.1 2005-02-19 21:03:46 crueda Exp

#ifndef Csv_h
#define Csv_h

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Csv {	// read and parse comma-separated values
	// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625

  public:
	Csv(istream& fin = cin, string sep = ",") : 
		fin(fin), fieldsep(sep) {}

	bool getline(string&);
	string getfield(string::size_type n);
	unsigned getnfield() const { return nfield; }

  private:
	istream& fin;			// input file pointer
	string line;			// input line
	vector<string> field;	// field strings
	unsigned nfield;				// number of fields
	string fieldsep;		// separator characters

	unsigned split();
	bool endofline(char);
	string::size_type advplain(const string& line, string& fld, int);
	string::size_type advquoted(const string& line, string& fld, int);
};

#endif
