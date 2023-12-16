#!/bin/sh
echo "public class $1" > bin/$1.java
echo "{" >> bin/$1.java
echo "	public static void main(String []args)" >> bin/$1.java
echo "	{" >> bin/$1.java
echo "		System.load($1.class.getProtectionDomain().getCodeSource().getLocation().getPath()+\"$1.so\");" >> bin/$1.java
echo "		System.exit(MyMain.myMain(args));" >> bin/$1.java
echo "	}" >> bin/$1.java
echo "}" >> bin/$1.java

cp MyMain.java bin
cd bin
javac $1.java
cd ..


