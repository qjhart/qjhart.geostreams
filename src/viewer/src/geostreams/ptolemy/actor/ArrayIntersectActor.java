package geostreams.ptolemy.actor;

import geostreams.ptolemy.icon.OperatorIcon;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ArrayToken;
import ptolemy.data.DoubleToken;
import ptolemy.data.IntToken;
import ptolemy.data.Token;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.ArrayType;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Intersects two input arrays.
 * Optionally, some elements in the resulting arrays can be removed
 * as an easy mechanism to get rid of outliers.
 * (This is all for my own quick tests).
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: ArrayIntersectActor.java,v 1.2 2007/07/30 09:04:06 crueda Exp $
 */
public class ArrayIntersectActor extends TypedAtomicActor {
	
	public TypedIOPort input0;
	public TypedIOPort input1;
	public TypedIOPort output0;
	public TypedIOPort output1;
	
	public TypedIOPort outputIndices;
	
	
	public StringParameter omitParam;
	private List<Integer> omit;
	
	
	protected OperatorIcon _icon;
	
	
    public ArrayIntersectActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new OperatorIcon(this, "_icon", "Inters");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		input0 = new TypedIOPort(this, "input0", true, false);
		input0.setTypeEquals(new ArrayType(BaseType.DOUBLE));

		input1 = new TypedIOPort(this, "input1", true, false);
		input1.setTypeEquals(new ArrayType(BaseType.DOUBLE));

		output0 = new TypedIOPort(this, "output0", false, true);
		output0.setTypeEquals(new ArrayType(BaseType.DOUBLE));
		output0.setMultiport(true);
		
		output1 = new TypedIOPort(this, "output1", false, true);
		output1.setTypeEquals(new ArrayType(BaseType.DOUBLE));
		output1.setMultiport(true);
		
		outputIndices = new TypedIOPort(this, "outputIndices", false, true);
		outputIndices.setTypeEquals(new ArrayType(BaseType.INT));
		outputIndices.setMultiport(true);
		
		omitParam = new StringParameter(this, "omitParam");
		omitParam.setExpression("");
		omitParam.setDisplayName("Omit indices (i,j,k...)");
	}
    
    public synchronized void initialize() throws IllegalActionException {
        super.initialize();
        String[] indices = omitParam.stringValue().split("\\s*,\\s*");
        Set<Integer> set = new HashSet<Integer>();
        for ( String index : indices ) {
        	if ( index.trim().length() > 0 ) {
        		set.add(Integer.parseInt(index.trim()));
        	}
        }
		omit = new ArrayList<Integer>(set);
		// sort in descending order, so removal of indices works correctly:
		Collections.sort(omit, new Comparator<Integer>() {
			public int compare(Integer o1, Integer o2) {
				return o2 - o1;
			}
		});
    }
    
	public void fire() throws IllegalActionException {
		Token[] inArray0 = ((ArrayToken) input0.get(0)).arrayValue();
		Token[] inArray1 = ((ArrayToken) input1.get(0)).arrayValue();

		int minLen = Math.min(inArray0.length, inArray1.length);
		
		List<Token> list0 = new ArrayList<Token>();
		List<Token> list1 = new ArrayList<Token>();
		List<Token> indices = new ArrayList<Token>();
		
		for (int i = 0; i < minLen; i++) {
			if ( !inArray0[i].isNil() && !inArray1[i].isNil() ) {
				list0.add(new DoubleToken( ((DoubleToken)inArray0[i]).doubleValue() ));
				list1.add(new DoubleToken( ((DoubleToken)inArray1[i]).doubleValue() ));
				indices.add(new IntToken(i));
			}
		}
		
		for ( int index : omit ) {
			// omit list is sorted in descending order obove.
			list0.remove(index);
			list1.remove(index);
			indices.remove(index);
		}
		
		int outputLen = list0.size();
		
		output0.broadcast(new ArrayToken(list0.toArray(new Token[outputLen])));
		output1.broadcast(new ArrayToken(list1.toArray(new Token[outputLen])));
		outputIndices.broadcast(new ArrayToken(indices.toArray(new Token[outputLen])));
	}
}
