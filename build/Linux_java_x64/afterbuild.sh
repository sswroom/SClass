#!/bin/sh
echo "public class $1" > $1.java
echo "{" >> $1.java
echo "	public static void main(String []args)" >> $1.java
echo "	{" >> $1.java
echo "		System.load($1.class.getProtectionDomain().getCodeSource().getLocation().getPath()+\"$1.so\");" >> $1.java
echo "		System.exit(MyMain.myMain(args));" >> $1.java
echo "	}" >> $1.java
echo "}" >> $1.java

javac $1.java

