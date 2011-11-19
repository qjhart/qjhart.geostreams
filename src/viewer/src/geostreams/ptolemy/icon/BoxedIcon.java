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
import diva.canvas.toolbox.BasicRectangle;
import diva.canvas.toolbox.LabelFigure;
import diva.gui.toolbox.FigureIcon;

/**
 * Adapted from Ptolemy's TextIcon and BoxedValuesIcon classes.
 * @author Carlos Rueda
 * @version $Id: BoxedIcon.java,v 1.1 2007/05/25 23:59:51 crueda Exp $
 */
public class BoxedIcon extends DynamicEditorIcon {
    public BoxedIcon(NamedObj container, String name)
            throws IllegalActionException, NameDuplicationException {
        super(container, name);
        setPersistent(false);
    }

    public Object clone(Workspace workspace) throws CloneNotSupportedException {
    	BoxedIcon newObject = (BoxedIcon) super.clone(workspace);
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

            // NOTE: Padding of 20.
            _width = stringBounds.getWidth() + 20;
            _heigth = stringBounds.getHeight() + 10;
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
        BasicRectangle result = new BasicRectangle(0, 0, _width, _heigth, _boxColor, 2);
    	_addLiveFigure(result);
        return result;
    }
    
    public Figure createFigure() {
    	CompositeFigure result = (CompositeFigure) super.createFigure();
    	
    	BasicRectangle fig = new BasicRectangle(new Rectangle2D.Double(1.0, 1.0, 50.0, 6.0));
    	fig.setFillPaint(Color.GRAY);
    	result.add(fig);
    	
    	
    	BasicFigure fig2 = new BasicFigure(new Ellipse2D.Double(0.0, 0.0, 50.0, 50.0));
    	result.add(fig2);
    	
    	LabelFigure label = _createLabelFigure();
    	
    	Rectangle2D backBounds = result.getBackgroundFigure().getBounds();
//        label.translateTo(backBounds.getCenterX(), backBounds.getCenterY());
    	label.translateTo(backBounds.getMinX() + 5, backBounds.getMinY() + 10);
        
    	result.add(label);
    	
    	_addLiveFigure(label);

    	return result;
    }

    /** Create a new Swing icon.  This returns an icon with the text
     *  "-A-", or if it has been called, the text specified by
     *  setIconText().
     *  @see #setIconText(String)
     *  @return A new Swing Icon.
     */
    public javax.swing.Icon createIcon() {
        // In this class, we cache the rendered icon, since creating icons from
        // figures is expensive.
        if (_iconCache != null) {
            return _iconCache;
        }

        // No cached object, so rerender the icon.
        LabelFigure figure = new LabelFigure(_iconText, _font);
        figure.setFillPaint(_textColor);
        _iconCache = new FigureIcon(figure, 20, 15);
        return _iconCache;
    }

    
    private void _update() {
        Runnable doSet = new Runnable() {
            public void run() {
                Iterator figures = _liveFigureIterator();

                while (figures.hasNext()) {
                    Object figure = figures.next();
                    if ( figure instanceof LabelFigure ) {
                    	((LabelFigure) figure).setFillPaint(_textColor);
                    	((LabelFigure) figure).setFont(_font);
                    	((LabelFigure) figure).setString(_displayString());
                    }
                    else if ( figure instanceof BasicRectangle ) {
                    	((BasicRectangle) figure).setFillPaint(_boxColor);
                    	((BasicRectangle) figure).setPrototypeShape(new Rectangle2D.Double(0.0, 0.0, _width, _heigth));
                    }
                }
            }
        };
        SwingUtilities.invokeLater(doSet);
    }
    
    /** Specify the text color to use.  This is deferred and executed
     *  in the Swing thread.
     *  @param textColor The fill color to use.
     */
    public void setTextColor(Color textColor) {
        _textColor = textColor;
        _update();
    }

    /** Specify the box color.  This is deferred and executed
     *  in the Swing thread.
     *  @param boxColor The fill color to use.
     */
    public void setBoxColor(Color boxColor) {
        _boxColor = boxColor;
        _update();
    }

    /** Specify the font to use.  This is deferred and executed
     *  in the Swing thread.
     *  @param font The font to use.
     */
    public void setFont(Font font) {
        _font = font;
        _update();
    }

    /** Specify the text to display in the icon.
     *  If this is not called, then the text displayed
     *  is "-A-".
     *  @param text The text to display in the icon.
     */
    public void setIconText(String text) {
        _iconText = text;
    }

    /** Specify text to display.  This is deferred and executed
     *  in the Swing thread.
     *  @param text The text to display.
     */
    public void setText(String text) {
        _text = text;
        _update();
    }

    ///////////////////////////////////////////////////////////////////
    ////                         private variables                 ////

    // The font to use.
    private Font _font = new Font("SansSerif", Font.PLAIN, 11);

    // Default text.
    private String _iconText = "-A-";

    // The specified text color.
    private Color _textColor = Color.BLACK;

    // The text that is rendered.
    private String _text = "";

    // box width
    private double _width = 60;
    
    // box height
    private double _heigth = 30;

    // box color.
    private Color _boxColor = Color.RED;

}
