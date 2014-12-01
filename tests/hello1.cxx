/*
 * hello1.cxx
 * 
 * Copyright 2014 Unknown <gabi@acere1>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>

using namespace std;
int main(int argc, char **argv)
{

	int buff[5][8]= {0};

	for (int j=0; j< 5; j++)
	{
		for (int i=0; i<8; i++)
		{
			buff[j][i] = 8 * i+j;
			cout << "See: "<<i <<" "<<j <<" "<< buff[j][i] << endl;
		}
	}
	
	return 0;
}

