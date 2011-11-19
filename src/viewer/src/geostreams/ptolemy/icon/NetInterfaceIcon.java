package geostreams.ptolemy.icon;

import geostreams.ptolemy.icon.Monitor.State;

import java.awt.Color;
import java.awt.Font;
import java.awt.geom.Rectangle2D;
import java.awt.geom.RoundRectangle2D;
import java.util.Iterator;

import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;

import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.kernel.util.NamedObj;
import ptolemy.kernel.util.Workspace;
import ptolemy.vergil.icon.DynamicEditorIcon;
import diva.canvas.CompositeFigure;
import diva.canvas.Figure;
import diva.canvas.toolbox.BasicFigure;
import diva.canvas.toolbox.LabelFigure;

/**
 * The icon for the NI component.
 * 
 * @author Carlos Rueda
 * @version $Id: NetInterfaceIcon.java,v 1.7 2007/06/27 02:33:27 crueda Exp $
 */
public class NetInterfaceIcon extends DynamicEditorIcon {
    public NetInterfaceIcon(NamedObj container, String name)
            throws IllegalActionException, NameDuplicationException {
        super(container, name);
        setPersistent(false);
        _monitor = new Monitor();
    }

    public Object clone(Workspace workspace) throws CloneNotSupportedException {
    	NetInterfaceIcon newObject = (NetInterfaceIcon) super.clone(workspace);
        return newObject;
    }
    
    
    private String _displayString() {
    	String displayString = _text;
        if (displayString != null) {
            // Measure width of the text.  Unfortunately, this
            // requires generating a label figure that we will not use.
            LabelFigure label = new LabelFigure(displayString, _font, 1.0,
                    SwingConstants.CENTER);
            Rectangle2D stringBounds = label.getBounds();

            _width = stringBounds.getWidth() + 30;
            _heigth = stringBounds.getHeight() + 20;
        }
        return displayString;
    }

    private LabelFigure _createLabelFigure() {
    	LabelFigure label = new LabelFigure(_displayString(), _font);

        // By default, the origin should be the upper left.
        label.setAnchor(SwingConstants.NORTH_WEST);
        label.setFillPaint(_textColor);

        return label;
    }

    /** Create a new default background figure.
     *  @return A figure representing the specified shape.
     */
    public Figure createBackgroundFigure() {
    	_displayString();
    	BasicFigure fig = new BasicFigure(new RoundRectangle2D.Double(0.0, 0.0, _width, _heigth, 8, 8));
    	fig.setFillPaint(_boxColor);
        return fig;
    }
    
    public Figure createFigure() {
    	CompositeFigure result = (CompositeFigure) super.createFigure();
    	
    	final double radius = 4;
    	final int numPoints = 7;
    	_monitor.setProgressBarFigure(numPoints, 
//    			(_width - numPoints*radius)/2,
    			_width - numPoints*radius - 2,
    			_heigth - radius - 2, numPoints*radius, radius);
    	_addLiveFigure(_monitor.getFigure());
    	result.add(_monitor.getFigure());
    	
    	LabelFigure label = _createLabelFigure();
    	
    	Rectangle2D backBounds = result.getBackgroundFigure().getBounds();
    	label.translateTo(backBounds.getMinX() + 10, backBounds.getMinY() + 6);
    	_addLiveFigure(label);
        
    	result.add(label);
    	
    	return result;
    }

     public void update(boolean active, int numClients, int numServers) {
    	_text = "clients: " +numClients+ "\n" +
    	        "streams: " +numServers;
		if ( active ) {
			_monitor.setState(State.RUNNING);
		}
		else {
			_monitor.setState(State.IDLE);
		}
		_update();
    }
    
     
    public void setBackgroundColor(Color color) {
    	_boxColor = color;
    	_update();
    }
    
    
    private void _update() {
        Runnable doSet = new Runnable() {
            public void run() {
                Iterator figures = _liveFigureIterator();

                while (figures.hasNext()) {
                    Object figure = figures.next();
                    if ( figure instanceof LabelFigure ) {
                    	((LabelFigure) figure).setString(_displayString());
                    }
                    else if ( figure instanceof Monitor.MFigure ) {
                    	((Monitor.MFigure) figure).update();
                    }
                }
            }
        };
        SwingUtilities.invokeLater(doSet);
    }
    

    ///////////////////////////////////////////////////////////////////
    ////                         private variables                 ////

    private Font _font = new Font("Monospaced", Font.PLAIN, 9);

    private Color _textColor = Color.BLACK;

    private String _text = "clients: " +"0"+ "\n" +
    					"streams: " +"0";

    private double _width = 60;
    
    private double _heigth = 30;

    private Color _boxColor = Color.LIGHT_GRAY;
    
    private Monitor _monitor;
}
