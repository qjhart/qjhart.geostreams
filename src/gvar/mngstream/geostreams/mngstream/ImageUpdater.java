/*
	GeoStreams Project
	ImageUpdater
	Carlos A. Rueda
	$Id: ImageUpdater.java,v 1.2 2004/07/28 01:15:14 crueda Exp $
*/
package geostreams.mngstream;

import geostreams.goes.GOESConstants;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import java.util.List;

/**
	A runnable object that reads a mng stream and updates
	a cumulative image.
 */
public class ImageUpdater extends Thread {
	private MngInputStream mng;
	private CumulativeImage cumImg;
	private MngInputStream.Png png = new MngInputStream.Png();

	/** Creates a cumulative image updater. */
	public ImageUpdater(MngInputStream mng, CumulativeImage cumImg) {
		super("ImageUpdater");
		this.mng = mng;
		this.cumImg = cumImg;
	}

	public void run() {
		try {
			System.out.println("getting images");
			while ( mng.nextPng(png) ) {
				cumImg.addPng(png);
			}
		}
		catch (IOException ex) {
			System.out.println("ImageUpdater: IOException: " +ex.getMessage());
		}
	}
}

