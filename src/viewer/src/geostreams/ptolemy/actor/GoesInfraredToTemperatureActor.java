package geostreams.ptolemy.actor;

import geostreams.goes.TemperatureConversion;
import geostreams.ptolemy.icon.OperatorIcon;

import java.awt.Color;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ArrayToken;
import ptolemy.data.DoubleToken;
import ptolemy.data.Token;
import ptolemy.data.type.ArrayType;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/** 
 * Raw infrared count to temperature conversion.
 * 
 * See {@link http://www.oso.noaa.gov/goes/goes-calibration/gvar-conversion.htm}
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: GoesInfraredToTemperatureActor.java,v 1.3 2007/08/23 09:20:45 crueda Exp $
 */
public class GoesInfraredToTemperatureActor extends TypedAtomicActor {
	
	public TypedIOPort input;
	public TypedIOPort output;
	
	protected OperatorIcon _icon;
	
	// TODO provide a parameter to choose the infrared channel to use.
	
	private TemperatureConversion _tempConv = new TemperatureConversion(2);
	
	
    public GoesInfraredToTemperatureActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new OperatorIcon(this, "_icon", "I2T");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		input = new TypedIOPort(this, "input", true, false);
		input.setTypeEquals(new ArrayType(BaseType.DOUBLE));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
//		output.setTypeEquals(BaseType.DOUBLE);
		output.setMultiport(true);
		
	}
    
    
	public void fire() throws IllegalActionException {
		if ( input.getWidth() == 0 || !input.hasToken(0) ) {
			return;
		}
		ArrayToken token = (ArrayToken) input.get(0);
		Token[] inArray = token.arrayValue();

		Token[] array = new Token[inArray.length];
			
		for (int i = 0; i < inArray.length; i++) {
			DoubleToken doubleToken = (DoubleToken) inArray[i];
			if ( doubleToken == null || doubleToken.isNil() ) {
				array[i] = DoubleToken.NIL;
			}
			else {
				double x = doubleToken.doubleValue();
				double t = _tempConv.convert(x);
				array[i] = new DoubleToken(t);
			}
		}
		
		output.broadcast(new ArrayToken(array));
	}
}
