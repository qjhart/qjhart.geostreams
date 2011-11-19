package geostreams.ptolemy.actor;

import geostreams.ptolemy.icon.OperatorIcon;

import java.awt.Color;
import java.util.LinkedHashMap;
import java.util.Map;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ArrayToken;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.ArrayType;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.Attribute;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * This actor makes a spatial conversion of a point set.
 * 
 * <p>
 * TODO Currently, this is not implemented. Input token is output
 * without any modification.
 * 
 * <p>
 * TODO Change the name of this class.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: LatLonToGoesProjectionActor.java,v 1.3 2007/09/16 09:47:03 crueda Exp $
 */
public class LatLonToGoesProjectionActor extends TypedAtomicActor {
	private static class Conversion {
	    String code;
        String shortName;
        Conversion(String code, String shortName) {
            this.code = code;
            this.shortName = shortName;
        }
    }
    
    /** shortName -&gt; Conversion */
	private static final Map<String,Conversion> _convs = new LinkedHashMap<String,Conversion>();
    private static final Conversion L2G = new Conversion("L2G", "LatLon -> GOES");
    private static final Conversion G2L = new Conversion("G2L", "GOES -> LatLon");
    private static final Conversion A2L = new Conversion("A2L", "Albers Equal Area -> LatLon");
    private static final Conversion L2A = new Conversion("L2A", "LatLon -> Albers Equal Area");
    static {
        _convs.put(L2G.shortName, L2G);
        _convs.put(G2L.shortName, G2L);
        _convs.put(A2L.shortName, A2L);
        _convs.put(L2A.shortName, L2A);
    }
    
	public TypedIOPort input;
	public TypedIOPort output;
	
	public StringParameter conversionParam;
	
	protected OperatorIcon _icon;
	
	
    public LatLonToGoesProjectionActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new OperatorIcon(this, "_icon", "L2G");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		input = new TypedIOPort(this, "input", true, false);
		input.setTypeEquals(new ArrayType(BaseType.DOUBLE));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
//		output.setTypeEquals(BaseType.DOUBLE);
		output.setMultiport(true);
		
		conversionParam = new StringParameter(this, "dateParam");
        Conversion[] vals = _convs.values().toArray(new Conversion[_convs.size()]);
		conversionParam.setExpression(vals[0].shortName);
        for (Conversion c : vals) {
            conversionParam.addChoice(c.shortName);
        }
		conversionParam.setDisplayName("Conversion");
		
	}

    public void attributeChanged(Attribute attribute) throws IllegalActionException {
		if (attribute == conversionParam) {
	        String shortName = conversionParam.stringValue();
            Conversion c = _convs.get(shortName);
            if ( c == null ) {
                throw new IllegalActionException("Unrecognized conversion");
            }
	        _icon.setText(c.code);
		}
		else {
			super.attributeChanged(attribute);
		}
	}
    
	public void fire() throws IllegalActionException {
		if ( input.getWidth() == 0 || !input.hasToken(0) ) {
			return;
		}
		ArrayToken token = (ArrayToken) input.get(0);
		// ... no conversion yet.
		output.broadcast(token);
	}
}
