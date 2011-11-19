package geostreams.goes;

import java.util.HashMap;
import java.util.Map;

/**
 * Raw infrared count to temperature (celcius) conversion.
 * 
 * See {@link http://www.oso.noaa.gov/goes/goes-calibration/gvar-conversion.htm}
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: TemperatureConversion.java,v 1.1 2007/08/23 09:20:45 crueda Exp $
 */
public class TemperatureConversion {
	
	/** The specific set of constants to use according to infrared channel */
	private ChannelCoeffs cc;
	

	/**
	 * Creates a converter.
	 * @param channel Imager channel to be used. Note that the "first" infrared channel is 2. 
	 * @throws IllegalArgumentException If channel is not valid.
	 */
	public TemperatureConversion(int channel) {
		cc = chCoeffsMap.get(channel);
		if ( cc == null ) {
			throw new IllegalArgumentException("Channel must be one of " +chCoeffsMap.keySet());
		}
	}
	
	/** 
	 * Gets the temperature (celcius) corresponding to a raw infrared count.
	 * 
	 * @param x Raw infrared count
	 * @return Temperature (celcius)
	 */
	public double convert(double x) {
		
		// Conversion of Imager GVAR Count to Scene Radiance
		// R = (X - b)/m
		double r = ( x - cc.b ) / cc.m;
		
		// Conversion from radiance to effective temperature
		// T_eff = (c2 * n ) / ln [1 + (c1 * n^3) / R]
		double t_eff = (c2 * cc.n) / Math.log(1 + c1*cc.n3 / r);
		
		
		// Convert effective temperature Teff to actual temperature T (K)
		// T = a + b * T_eff
		double t_kelvin = cc.aa + cc.bb * t_eff;
		
		
		//Celsius = kelvin - 273.15
		double t = t_kelvin - 273.15;
		
		return t;
	}
    

	static class ChannelCoeffs {
		double m;
		double b;
		double n;
		double aa;
		double bb;
		
		double n3;  // cube(n)
		
		ChannelCoeffs(double m, double b, double n, double aa, double bb) {
			this.m = m;
			this.b = b;
			
			this.n = n;
			this.aa = aa;
			this.bb = bb;
			
			this.n3 = n * n * n;
		}
	}
	
	
	private static final double c1 = 1.191066e-5;
	private static final double c2 = 1.438833;
	
	
	private static Map<Integer,ChannelCoeffs> chCoeffsMap = new HashMap<Integer,ChannelCoeffs>();
	static {
		// From Table 1-1. GOES-8 through -11 Imager Scaling Coefficients
		//  and Table 2-4. GOES-11 Imager (Side 1) Coefficients
		chCoeffsMap.put(2, new ChannelCoeffs(227.3889, 68.2167, 2562.07, -0.644790, 1.000775));
		chCoeffsMap.put(3, new ChannelCoeffs( 38.8383, 29.1287, 1481.53, -0.543401, 1.001495));
		chCoeffsMap.put(4, new ChannelCoeffs(  5.2285, 15.6854,  931.76, -0.306809, 1.001274));
		chCoeffsMap.put(5, new ChannelCoeffs(  5.0273, 15.3332,  833.67, -0.333216, 1.001000));
	}
}
