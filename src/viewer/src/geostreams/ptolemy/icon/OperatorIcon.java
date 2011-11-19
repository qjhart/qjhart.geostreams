package geostreams.ptolemy.icon;

import java.awt.Color;
import java.awt.Font;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;
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
 * A generic icon for operators.
 * 
 * @author Carlos Rueda
 * @version $Id: OperatorIcon.java,v 1.9 2007/06/23 18:38:29 crueda Exp $
 */
public class OperatorIcon extends DynamicEditorIcon {
	public OperatorIcon(NamedObj container, String name, String text)
            throws IllegalActionException, NameDuplicationException {
        super(container, name);
        this._text = text;
        if ( text != null && text.length() == 1 ) {
        	_font = new Font("SansSerif", Font.PLAIN, 20);
        }
        _minStringBounds = _getStringBounds("=", _font);
    }

    public Object clone(Workspace workspace) throws CloneNotSupportedException {
    	OperatorIcon newObject = (OperatorIcon) super.clone(workspace);
        return newObject;
    }
    
    
    private static Rectangle2D _getStringBounds(String string, Font font) {
    	Rectangle2D stringBounds = new Rectangle2D.Float();
        if (string != null) {
            LabelFigure label = new LabelFigure(string, font, 1.0, SwingConstants.CENTER);
            stringBounds.setRect(label.getBounds());
        }
    	return stringBounds;
    }
    
    
    private String _displayString() {
    	String displayString = _text;
        if (displayString != null) {
        	if ( displayString.length() == 1 ) {
    			_width = _minStringBounds.getWidth();
    			_heigth = _minStringBounds.getHeight();
        	}
        	else {
        		Rectangle2D stringBounds = _getStringBounds(displayString, _font);

        		_width = stringBounds.getWidth();
        		_heigth = stringBounds.getHeight();

        		if ( _width < _minStringBounds.getWidth() ) {
        			_width = _minStringBounds.getWidth();
        		}
        		if ( _heigth < _minStringBounds.getHeight() ) {
        			_heigth = _minStringBounds.getHeight();
        		}
        	}
        	
        	if ( _heigth > _width ) {
        		_width = _heigth;
        	}
        	
            // NOTE: Padding of 20.
            _width += 20;
            _heigth += 10;
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

    public Figure createBackgroundFigure() {
    	_createLabelFigure();
    	BasicFigure fig = new BasicFigure(new Ellipse2D.Double(0.0, 0.0, _width, 1.7*_heigth));
    	fig.setFillPaint(_bgColor);
    	fig.setStrokePaint(_bgColor);
//    	_addLiveFigure(fig);
        return fig;
    }
    
    public Figure createFigure() {
    	CompositeFigure result = (CompositeFigure) super.createFigure();
    	
    	LabelFigure label = _createLabelFigure();
    	
    	Rectangle2D backBounds = result.getBackgroundFigure().getBounds();
    	Rectangle2D labelBounds = label.getBounds();
    	
//    	label.translateTo(backBounds.getMinX() + 8, backBounds.getMinY() + (1.7/2)*12);
    	label.translateTo(
    			backBounds.getMinX() + (backBounds.getWidth() - labelBounds.getWidth()) / 3, 
    			backBounds.getMinY() + (backBounds.getHeight() - labelBounds.getHeight()) / 3
    	);
    	
    	result.add(label);
    	
    	_addLiveFigure(label);
    	
    	return result;
    }

    public void setFont(Font font) {
        _font = font;
        _minStringBounds = _getStringBounds("=", _font);
        
        _update();
    }

    public void setText(String text) {
        _text = text;
        _update();
    }
    
    public void setBackgroundColor(Color color) {
    	_bgColor = color;
        _update();
    }

    private void _update() {
        Runnable doSet = new Runnable() {
            public void run() {
                Iterator figures = _liveFigureIterator();

                while (figures.hasNext()) {
                    Object figure = figures.next();
                    if ( figure instanceof LabelFigure ) {
                    	((LabelFigure) figure).setFont(_font);
                    	((LabelFigure) figure).setString(_displayString());
                    }
                }
            }
        };
        SwingUtilities.invokeLater(doSet);
    }

    
    ///////////////////////////////////////////////////////////////////
    ////                         private variables                 ////
    
    private Font _font = new Font("SansSerif", Font.PLAIN, 12);

    private Rectangle2D _minStringBounds;
    
    private Color _bgColor = Color.LIGHT_GRAY;

    private Color _labelColor = Color.WHITE; //BLACK;

    private String _text = "Op";

    private double _width = 60;
    
    private double _heigth = 30;
}
