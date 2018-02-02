#include <fstream>
#include <iostream>
#include <string>
#include <assert.h>
using namespace std;
typedef unsigned int uint;
typedef unsigned char byte;
 
const size_t TABLE_SIZE=256;
const size_t TABLE_WIDTH=8;
const size_t TABLE_DEPTH=32;

char table[TABLE_SIZE][TABLE_DEPTH][TABLE_WIDTH]={0};

string compress(string in)
{
	byte a=0;
	byte b=0;

	size_t slot=0;
	size_t len=0;
	size_t depth=0;

	string ret;
	for(size_t i=0;i<in.length();) {
		char c=in[i];
		/*char next=0;
		if(i+1<in.length()) {
			next=in[i+1];
		}*/
		//slot=(size_t)(byte)c;
		//slot+=((size_t)next)/32*128;
		//slot%=TABLE_SIZE;

		byte c2=(byte)c;
		size_t start_slot = ((size_t)c2)%16*16;

		size_t longestd=0;
		size_t longestlen=0;
		size_t longestslot=0;//(size_t)c;

		for(slot=start_slot;slot<start_slot+16;slot++) {
			for(size_t d=0;d<TABLE_DEPTH;d++) {
				for(len=0;len<TABLE_WIDTH && len+i<in.length();len++) {
					c=in[i+len];
					if(table[slot][d][len]==c) {
					} else if(table[slot][d][len]==0) {
						//table[slot][d][len]=c;
						//len++;
						//break;
					} else {
						//len++;
						break;
					}
				}
				//len--;

				if(len>longestlen) {
					longestd=d;
					longestlen=len;
					longestslot=slot;
				}
			}
		}

		len=longestlen-1;
		depth=longestd;
		slot=longestslot;

		for(size_t j=0;j<=len;j++) {
			if(table[slot][depth][j]==0) {
				table[slot][depth][j]=in[i+j];
			}
		}

		a=(byte)slot;
		//b=(byte)(len&0xF)+(depth<<4);
		b=(byte)(len&TABLE_WIDTH-1)+(depth*TABLE_WIDTH);
		ret += ((char)a);
		ret += ((char)b);
		i+=len+1;
	}

	return ret;
}

string uncompress(string in)
{
	string ret;
	for(size_t i=0;i<in.length();i+=2) {
		byte slot = (byte)in[i];
		byte len = (byte)in[i+1];
		//byte depth = len>>4;
		//len &= 0xF;
		byte depth = len/TABLE_WIDTH;
		len &= TABLE_WIDTH-1;
		len++;
		string block = string( table[slot][depth], len );
		ret += block;
	}
	return ret;
}

int main()
{
	for(size_t i=0;i<256;i++) {
		//table[i][0][0]=i;
		//table[i][0][1]=0;
		table[i%16*16][i/16][0]=i;
	}
	//strcpy(table['a'], "artist");

	ifstream file("D:\\discogs dumps\\discogs_20121201_relevant_artists.xml");
	size_t buffsize=1024*1024*16;
	char *buffer = new char[buffsize];
	file.read(buffer, buffsize);
	buffer[file.gcount()]='\0';
	string uncompressed=buffer;
	delete[] buffer;

	//uncompressed="hello 1234 test hello test";

	cout << "  uncompressed.length() == \t"<<uncompressed.length()<<"\n";
	string compressed = compress(uncompressed);
	cout << "    compressed.length() == \t"<<compressed.length()<<"\n";
	string reuncompressed = uncompress(compressed);
	cout << "reuncompressed.length() == \t"<<reuncompressed.length()<<"\n";
	cout << reuncompressed.substr(0,50) << "\n\n";

	for(size_t i=0;i<reuncompressed.length() && i<uncompressed.length(); i++) {
		if(reuncompressed[i]!=uncompressed[i]) {
			cout << reuncompressed.substr(i-10, 20) << "\n";
		}
	}
	assert(uncompressed == reuncompressed);
	return 0;
}