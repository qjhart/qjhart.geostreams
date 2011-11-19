package geostreams.ptolemy.icon;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Ellipse2D;
import java.util.HashMap;
import java.util.Map;

import javax.swing.SwingUtilities;
import javax.swing.Timer;

import diva.canvas.CompositeFigure;
import diva.canvas.toolbox.BasicFigure;

/**
 * A monitor keeps a state and has an associated visual component for activity feedback.
 * 
 * <p>
 * NOTE: Very preliminary
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: Monitor.java,v 1.3 2007/07/21 02:36:11 crueda Exp $
 */
public class Monitor {

	/** 
	 * A monitor is always in one of the these states.
	 */
	public enum State {
		/** The monitored object is doing nothing. */
		IDLE,
		
		/** the monitored object is running. */
		RUNNING, 
		
		/** the monitored object is waiting for data. */
		WAITING, 
		
		/** some error has ocurred to the monitored object. */
		ERROR
	}
	
	public Monitor() {
		setStateColor(State.IDLE, Color.GRAY);
		setStateColor(State.RUNNING, Color.GREEN);
		setStateColor(State.WAITING, Color.YELLOW);
		setStateColor(State.ERROR, Color.RED);
	}
	
	/**
	 * Associates a color to a state. The visual effect depends on the
	 * particular monitor.
	 * @param state
	 * @param color
	 */
	public void setStateColor(State state, Color color) {
		_stateColors.put(state, color);
	}
	
	/**
	 * Sets the current state of this monitor.
	 * @param state
	 */
	public void setState(State state) {
		if ( _currentState == state ) {
			return;
		}
		
		_currentState = state;
		
		if ( _currentState != State.IDLE ) {
			figure.startTimer();
		}
		else {
			figure.stopTimer();
		}
		updateFigure();
	}
	
	public void setSimpleFigure(double x, double y, double w, double h) {
		figure = new Simple(x, y, w, h);
		figure._update();
	}

	public void setTrafficLightFigure(double x, double y, double w, double h) {
		figure = new TrafficLight(x, y, w, h);
		figure._update();
	}

	public void setTrafficLight2Figure(double x, double y, double w, double h) {
		figure = new TrafficLight2(x, y, w, h);
		figure._update();
	}

	public void setProgressBarFigure(int numPoints, double x, double y, double w, double h) {
		figure = new ProgressBar(numPoints, x, y, w, h);
		figure._update();
	}

	public CompositeFigure getFigure() {
		return figure;
	}

	/**
	 * Updates the GUI component according to the current state of this monitor.
	 */
	public void updateFigure() {
		figure.update();
	}
	
	
	/**
	 * Creates a timer that calls setState(state) if
	 * current time minus _lastRead is greater than the given delay.
	 */
	private Timer _createTimer(final int delay, final State state) {
		return new Timer(delay,  new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				long current = System.currentTimeMillis();
				long curr_delay = current - _lastRead;
				if ( curr_delay > delay ) {
					setState(state);
				}
			}
		});
	}
	
	abstract class MFigure extends CompositeFigure {
		protected abstract void _update();
		
		/**
		 * Updates the GUI according to the current state of this monitor.
		 */
		public void update() {
			_lastRead = System.currentTimeMillis();
	        Runnable doSet = new Runnable() {
	            public void run() {
	            	_update();
	            }
	        };
	        SwingUtilities.invokeLater(doSet);
		}
		
		void startTimer() {
			if ( _timer == null ) {
				_timer = _createTimer(1000, State.WAITING);
				_timer.start();
			}
		}
		void stopTimer() {
			if ( _timer != null ) {
				_timer.stop();
				_timer = null;
			}
		}
	}
	

	/**
	 * A simple monitor consisting of an single ellipse.
	 */
	private class Simple extends MFigure {
		private BasicFigure fig;
		
		public Simple(double x, double y, double w, double h) {
	    	fig = new BasicFigure(new Ellipse2D.Double(x, y, w, h));
	    	this.add(fig);
		}
		
		protected void _update() {
			fig.setFillPaint(_stateColors.get(_currentState));
		}
	}
	
	/**
	 * A "traffic light" monitor.
	 */
	private class TrafficLight extends MFigure {
		private BasicFigure[] figs = new BasicFigure[3];
		
		public TrafficLight(double x, double y, double w, double h) {
			// Assumed vertical layout:
			double figw = w;
			double figh = h / 3;
			double incX = 0;
			double incY = figh;
			if ( w > h ) {
				// correct: it'll be horizontal:
				figw = w / 3;
				figh = h;
				incX = figw;
				incY = 0;
			}
			for (int i = 0; i < figs.length; i++) {
				figs[i] = new BasicFigure(new Ellipse2D.Double(x + i*incX, y + i*incY, figw, figh));
				this.add(figs[i]);
			}
		}
		
		protected void _update() {
			for (int i = 0; i < figs.length; i++) {
				figs[i].setFillPaint(_stateColors.get(State.IDLE));
			}
			switch ( _currentState ) {
				case RUNNING:
					figs[2].setFillPaint(_stateColors.get(State.RUNNING));
					break;
				case WAITING:
					figs[1].setFillPaint(_stateColors.get(State.WAITING));
					break;
				case ERROR:
					for (int i = 0; i < figs.length; i++) {
						figs[i].setFillPaint(_stateColors.get(State.ERROR));
					}
					break;
			}
		}
	}
	
	/**
	 * A "traffic light" monitor with 2 lights.
	 */
	private class TrafficLight2 extends MFigure {
		private BasicFigure[] figs = new BasicFigure[2];
		
		public TrafficLight2(double x, double y, double w, double h) {
			// Assumed vertical layout:
			double figw = w;
			double figh = h / 2;
			double incX = 0;
			double incY = figh;
			if ( w > h ) {
				// correct: it'll be horizontal:
				figw = w / 2;
				figh = h;
				incX = figw;
				incY = 0;
			}
			for (int i = 0; i < figs.length; i++) {
				figs[i] = new BasicFigure(new Ellipse2D.Double(x + i*incX, y + i*incY, figw, figh));
				this.add(figs[i]);
			}
		}
		
		protected void _update() {
			for (int i = 0; i < figs.length; i++) {
				figs[i].setFillPaint(_stateColors.get(State.IDLE));
			}
			switch ( _currentState ) {
				case RUNNING:
					figs[0].setFillPaint(_stateColors.get(State.RUNNING));
					break;
				case WAITING:
					figs[1].setFillPaint(_stateColors.get(State.WAITING));
					break;
				case ERROR:
					for (int i = 0; i < figs.length; i++) {
						figs[i].setFillPaint(_stateColors.get(State.ERROR));
					}
					break;
			}
		}
	}
	
	/**
	 * A "progress bar" monitor.
	 */
	private class ProgressBar extends MFigure {
		private BasicFigure[] figs;
		private int currentIndex;
		
		public ProgressBar(int numPoints, double x, double y, double w, double h) {
			figs = new BasicFigure[numPoints];
			// Assumed vertical layout:
			double figw = w;
			double figh = h / numPoints;
			double incX = 0;
			double incY = figh;
			if ( w > h ) {
				// correct: it'll be horizontal:
				figw = w / numPoints;
				figh = h;
				incX = figw;
				incY = 0;
			}
			for (int i = 0; i < figs.length; i++) {
				figs[i] = new BasicFigure(new Ellipse2D.Double(x + i*incX, y + i*incY, figw, figh));
				this.add(figs[i]);
			}
		}
		
		void startTimer() {
			if ( _timer == null ) {
				_timer = new Timer(600, new ActionListener() {
					public void actionPerformed(ActionEvent ev) {
						currentIndex = (currentIndex + 1) % figs.length;
						update();
					}
				});
				_timer.start();
			}
		}
		
		protected void _update() {
			for (int i = 0; i < figs.length; i++) {
				if ( _currentState != State.IDLE ) {
					if ( i == currentIndex ) {
						figs[i].setFillPaint(_stateColors.get(State.WAITING));
					}
					else {
						figs[i].setFillPaint(_stateColors.get(State.RUNNING));
					}
				}
				else {
					figs[i].setFillPaint(_stateColors.get(State.IDLE));
				}
			}
		}
	}
	
	private State _currentState = State.IDLE;
	
	private Map<State,Color> _stateColors = new HashMap<State,Color>();
	
	private Timer _timer;
	private long _lastRead = 0;
	
	private MFigure figure;

}
