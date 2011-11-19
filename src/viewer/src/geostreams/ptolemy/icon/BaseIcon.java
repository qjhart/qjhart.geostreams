package geostreams.ptolemy.icon;

import geostreams.ptolemy.icon.Monitor.State;

import java.awt.Color;
import java.awt.Font;
import java.awt.geom.Rectangle2D;
import java.awt.geom.RoundRectangle2D;

import javax.swing.SwingConstants;

import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.kernel.util.NamedObj;
import ptolemy.vergil.icon.DynamicEditorIcon;
import diva.canvas.CompositeFigure;
import diva.canvas.Figure;
import diva.canvas.toolbox.BasicFigure;
import diva.canvas.toolbox.LabelFigure;

/**
 * The icon for the image stream reader actor.
 * 
 * @author Carlos Rueda
 * @version $Id: BaseIcon.java,v 1.4 2007/08/01 02:47:34 crueda Exp $
 */
public abstract class BaseIcon extends DynamicEditorIcon {
	protected Monitor _monitor;

	protected BaseIcon(NamedObj container, String name, String text)
            throws IllegalActionException, NameDuplicationException {
        super(container, name);
        _text = text;
        setPersistent(false);
        _monitor = new Monitor();
    }

    private String _displayString() {
    	String displayString = _text;
        if (displayString != null) {
            // Measure width of the text.  Unfortunately, this
            // requires generating a label figure that we will not use.
            LabelFigure label = new LabelFigure(displayString, _font, 1.0,
                    SwingConstants.CENTER);
            Rectangle2D stringBounds = label.getBounds();

            _width = stringBounds.getWidth() + 20;
            _heigth = stringBounds.getHeight() + 12;
        }
        return displayString;
    }

    private LabelFigure _createLabelFigure() {
    	LabelFigure label = new LabelFigure(_displayString(), _font);

        // By default, the origin should be the upper left.
        label.setAnchor(SwingConstants.NORTH_WEST);
        label.setFillPaint(_labelColor);

        return label;
    }

    /** Create a new default background figure.
     *  @return A figure representing the specified shape.
     */
    public Figure createBackgroundFigure() {
    	_createLabelFigure();
//    	BasicFigure fig = new BasicFigure(new Ellipse2D.Double(0.0, 0.0, _width, 1.7*_heigth));
    	BasicFigure fig = new BasicFigure(new RoundRectangle2D.Double(0.0, 0.0, _width, _heigth, 8, 8));
    	fig.setFillPaint(_bgColor);
    	fig.setStrokePaint(_bgColor);
        return fig;
    }
    
    /**
     * Creates the figure to associate with my monitor.
     * Nothing is done in this base class.
     */
    protected void _createMonitorFigure() {
    }
    
    public Figure createFigure() {
    	CompositeFigure result = (CompositeFigure) super.createFigure();
    	
    	LabelFigure label = _createLabelFigure();
    	
    	Rectangle2D backBounds = result.getBackgroundFigure().getBounds();
    	label.translateTo(backBounds.getMinX() + 11, backBounds.getMinY() + _heigth/4);
    	result.add(label);
    	
    	_createMonitorFigure();
    	CompositeFigure monFigure = _monitor.getFigure();
    	if ( monFigure != null ) {
	        _addLiveFigure(monFigure);
	        result.add(monFigure);
    	}
    	
    	return result;
    }

    public void setBackgroundColor(Color color) {
    	_bgColor = color;
    }

    /** 
     * Sets whether the icon is active or not.
     * @param active
     */
	public void setIconActive(boolean active) {
		_monitor.setState(active ? State.WAITING : State.IDLE);
	}

    /** 
     * Sets whether the input stream is active or not.
     * @param active
     */
	public void setStreamIsActive(boolean active) {
		_monitor.setState(active ? State.RUNNING : State.WAITING);
	}

	/** Sets the text for this icon */
	public void setText(String _text) {
		this._text = _text;
	}

	/** Sets the text font for this icon */
	public void setFont(Font font) {
		this._font = font;
	}

	protected double _width = 60;
	
	protected double _heigth = 30;

	///////////////////////////////////////////////////////////////////
    ////                         private variables                 ////

    private Font _font = new Font("SansSerif", Font.PLAIN, 12);

    private Color _bgColor = Color.LIGHT_GRAY;

    private Color _labelColor = Color.WHITE;//BLACK;

    private String _text = "";

}
