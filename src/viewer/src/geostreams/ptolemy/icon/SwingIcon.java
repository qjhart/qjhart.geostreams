package geostreams.ptolemy.icon;

import java.awt.Color;
import java.awt.Dimension;
import java.util.Iterator;

import javax.swing.JButton;
import javax.swing.JComponent;

import ptolemy.gui.Top;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.kernel.util.NamedObj;
import ptolemy.kernel.util.Workspace;
import ptolemy.vergil.icon.DynamicEditorIcon;
import diva.canvas.CanvasPane;
import diva.canvas.Figure;
import diva.canvas.FigureLayer;
import diva.canvas.JCanvas;
import diva.canvas.toolbox.BasicCanvasPane;
import diva.canvas.toolbox.SwingWrapper;
import diva.gui.toolbox.FigureIcon;

// just a quick test -- not used

public class SwingIcon extends DynamicEditorIcon {
   public SwingIcon(NamedObj container, String name)
           throws IllegalActionException, NameDuplicationException {
       super(container, name);
   }


   public Object clone(Workspace workspace) throws CloneNotSupportedException {
	   SwingIcon newObject = (SwingIcon) super.clone(workspace);
       return newObject;
   }

   private JComponent _createJComponent() {
	   JComponent jc;
	   jc = new JButton(_text);
//	   jc = new JProgressBar();
//	   jc = new JTextField(_text);
	   
	   jc.setPreferredSize(new Dimension(30, 50));
	   jc.setBackground(Color.red);
	   
	   return jc;
   }
   
   public Figure createBackgroundFigure() {
	   SwingWrapper figure = new SwingWrapper(_createJComponent());
	   CanvasPane canvasPanel = new BasicCanvasPane();
	   canvasPanel.setCanvas(new JCanvas());
	   FigureLayer layer = new FigureLayer(canvasPanel);
	   layer.add(figure);
       _addLiveFigure(figure);
       return figure;
   }

   public javax.swing.Icon createIcon() {
       if (_iconCache != null) {
           return _iconCache;
       }
	   SwingWrapper figure = new SwingWrapper(_createJComponent());
	   CanvasPane canvasPanel = new BasicCanvasPane();
	   canvasPanel.setCanvas(new JCanvas());
	   FigureLayer layer = new FigureLayer(canvasPanel);
	   layer.add(figure);
       _iconCache = new FigureIcon(figure, 20, 15);
       return _iconCache;
   }

   public void setText(String text) {
       _text = text;

       Runnable doSet = new Runnable() {
           public void run() {
               Iterator figures = _liveFigureIterator();

               while (figures.hasNext()) {
                   Object figure = figures.next();
                   ((SwingWrapper) figure).setComponent(new JButton(_text));
               }
           }
       };

       Top.deferIfNecessary(doSet);
   }

   private String _text;
}
