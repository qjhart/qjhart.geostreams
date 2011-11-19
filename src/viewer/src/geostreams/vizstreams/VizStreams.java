package geostreams.vizstreams;

import geostreams.goes.GOESConstants;
import geostreams.image.FrameStatus;
import geostreams.image.IConnectionObserver;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.ServerConnections;
import geostreams.image.StreamDef;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.GridLayout;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.ConnectException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;


/**
  * Main VizStreams panel. Tabs are used for the open streams.
  * Includes main() for a standalone program.
  * 		   
  * @author Carlos Rueda-Velasquez
  * @version $Id: VizStreams.java,v 1.4 2007/06/12 16:51:56 crueda Exp $
 */
public class VizStreams extends JPanel {
	/** associated server connection providing the data to be visualized */
	private IServerConnection gscon;
	
	/** Set the size of the canvas for the complete image. */
	final Dimension canvasSize;

	/** The real reference system. */
	private Rectangle referenceSpace;

	protected JToolBar _buttonsPanel = new JToolBar();
	
	private JButton buttonOpen = new JButton(new OpenAction());
	
	private JButton buttonClose = new JButton(new CloseAction());
	private JButton buttonStop = new JButton(new StopAction());
	
	private JButton buttonRoi = null;//new JButton(new RoiAction());
	
	private JPopupMenu popupRoi = new JPopupMenu();
	
	/** includeSaveAction() adds this button to the panel */
	private JButton buttonSave;

	private boolean autoSaveFrames;
	private boolean openAllStreams;
	
	/** Map: streamID -> ImageStreamViewer */
	private Map<String,ImageStreamViewer> openImgStreams;

	/** includeDevControls() stuff */
	static boolean developerTests = false;
	private boolean devControlsIncluded = false;
	private int simulationDelay = 0; // millis
	private int timeout = 0; // in seconds
	

	/** Load background image when opening a new stream ? */
	private static boolean loadBackground = true;

	/** a tab for each stream */
	JTabbedPane tabs;


	/** Overview window for the user to select ROI and stream. */
	private JFrame overview_jframe;
	
	private int initialZoomFactor = 40;
	
	private Overview overview = Overview.createOverview();

	
	/** Creates a Geostreams visualizer */
	public VizStreams(Dimension canvasSize) {
		super(new BorderLayout());
		this.canvasSize = (Dimension) canvasSize.clone();
		_buttonsPanel.setRollover(true);
		_buttonsPanel.setBorderPainted(false);
		add(_buttonsPanel, "North");
		tabs = new JTabbedPane();
		add(tabs, "Center");
		tabs.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				_updateButtons();
			}
		});
		_addButtons();
		_updateButtons();
	}

	private void _addButtons() {
		_buttonsPanel.add(buttonOpen);
		_buttonsPanel.add(buttonStop);
		_buttonsPanel.add(buttonClose);
		if ( buttonRoi != null ) {
			_buttonsPanel.add(buttonRoi);
		}
	}
	
	public void includeSaveAction() {
		if ( buttonSave == null ) {
			buttonSave = new JButton(new SaveAction());
			_buttonsPanel.add(buttonSave);
		}
		_updateButtons();
	}
	
	public void includeDevControls() {
		if ( !devControlsIncluded ) {
			devControlsIncluded = true;
			developerTests = true;

			final JButton buttonDeveloper = new JButton("Developer");
			_buttonsPanel.add(buttonDeveloper);
			buttonDeveloper.setToolTipText("Developer options");
			final JPopupMenu popupDeveloper = new JPopupMenu();
			buttonDeveloper.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					popupDeveloper.show(buttonDeveloper, 5, buttonDeveloper.getHeight());
				}
			});
			
			JPanel timeoutPanel = new JPanel(new GridLayout(1,0));
			popupDeveloper.add(timeoutPanel);
			final JLabel timeoutLabel = new JLabel("Connection timeout: 0m:0s", JLabel.LEFT);
			timeoutPanel.add(timeoutLabel);
			final JSlider timeoutSlider = new JSlider(JSlider.HORIZONTAL, 0, 15*60, 0);
			timeoutPanel.add(timeoutSlider);
			timeoutSlider.addChangeListener(new ChangeListener() {
				public void stateChanged(ChangeEvent e) {
					JSlider source = (JSlider)e.getSource();
					timeout = (int)source.getValue();
					if ( gscon != null ) {
						gscon.setTimeout(timeout);
					}
					int mins = timeout / 60;
					int secs = timeout % 60;
					timeoutLabel.setText("Connection timeout: " +mins+ "m:" +secs+ "s");
				}
			});


			JPanel simulationDelayPanel = new JPanel(new GridLayout(1,0));
			popupDeveloper.add(simulationDelayPanel);
			final JLabel simulationDelayLabel = new JLabel("Simulation delay: 000ms", JLabel.LEFT);
			simulationDelayPanel.add(simulationDelayLabel);
			final JSlider simulationDelaySlider = new JSlider(JSlider.HORIZONTAL, 0, 1000, 0);
			simulationDelayPanel.add(simulationDelaySlider);
			simulationDelaySlider.addChangeListener(new ChangeListener() {
				public void stateChanged(ChangeEvent e) {
					JSlider source = (JSlider)e.getSource();
					simulationDelay = (int)source.getValue();
					if ( gscon != null ) {
						gscon.setSimulationDelay(simulationDelay);
					}
					simulationDelayLabel.setText("Simulation delay: " +simulationDelay+ "ms");
				}
			});
		}
	}

	public static void setLoadBackground(boolean b) {
		loadBackground = b;
	}
	public static boolean getLoadBackground() {
		return loadBackground;
	}

	public void setAutoSaveFrames(boolean autoSaveFrames) {
		this.autoSaveFrames = autoSaveFrames;
	}

	public void setOpenAllStreams(boolean openAllStreams) {
		this.openAllStreams = openAllStreams;
	}
	
	boolean getOpenAllStreams() {
		return openAllStreams;
	}
	
	/** Updates the state of buttons. */
	private void _updateButtons() {
		ImageStreamViewer isv = (ImageStreamViewer) tabs.getSelectedComponent();
		buttonClose.setEnabled(isv != null);
		
		buttonStop.setEnabled(isv != null && gscon.isStreamRunning(isv.getStreamDef().getStreamID()));
		if ( buttonSave != null ) {
			buttonSave.setEnabled(isv != null);
		}
	}
	

	private ImageIcon _getIcon(String imageName) {
		String imgLocation = "geostreams/icons/" +imageName+ ".gif";
		URL imageURL = getClass().getClassLoader().getResource(imgLocation);
		return imageURL == null ? null : new ImageIcon(imageURL);
	}
	
	/** Main initialization method. 
	  */       
	public void init(IServerConnection gscon) {
		if ( gscon == null ) {
			throw new IllegalArgumentException("gscon == null");
		}
		this.gscon = gscon;
		
		openImgStreams = new HashMap<String,ImageStreamViewer>();
		
		buttonClose.setEnabled(false);
		
		//ToolTipManager.sharedInstance().setDismissDelay(5*60*1000);
	}

	/** Starts the user interface. You must call init() before.
	  * @throws IllegalStateException if init has not been called
	  */
	public void start() throws Exception {
		System.out.println("---- VizStreams start ----");
		if ( gscon == null ) {
			throw new IllegalStateException("gscon == null");
		}
		
		// reference space rectangle:
		referenceSpace = gscon.getReferenceSpace().getRectangle();
		
		if ( false) {
			_createPopupRoi();
		}
		
		gscon.startStreamReaders();
		
		if ( openAllStreams ) {
			for ( StreamDef streamDef : gscon.getStreamDefs() ) {
				Thread t = _getOpenStreamThread(streamDef, null);
				EventQueue.invokeLater(t);
			}
		}
		
	}
	
	private void _createPopupRoi() {
		// create ROI menu (currently very simple and no actual actions)
		JMenu menu1, menu2;
		popupRoi.add(new JMenuItem("Pixel/line box"));
		popupRoi.add(new JMenuItem("Latitude/longitude box"));
		popupRoi.addSeparator();
		menu1 = new JMenu("North America");
		popupRoi.add(menu1);
		menu1.add(new JMenu("Canada"));
		menu2 = new JMenu("U.S.A.");
		menu2.add(new JMenuItem("Conterminous U.S."));
		menu2.addSeparator();
		menu2.add(new JMenuItem("Oregon"));
		menu2.add(new JMenuItem("Idaho"));
		menu2.add(new JMenuItem("Montana"));
		menu2.add(new JMenuItem("California"));
		menu2.add(new JMenuItem("Nevada"));
		menu2.add(new JMenuItem("Utah"));
		menu2.add(new JMenu("Other states"));
		menu1.add(menu2);
		menu1.add(new JMenu("Mexico"));
		
		popupRoi.add(new JMenu("Central America"));
		popupRoi.add(new JMenu("South America"));
		popupRoi.addSeparator();
		popupRoi.add(new JMenu("Islands"));
		popupRoi.addSeparator();
		menu1 = new JMenu("Other regions");
		popupRoi.add(menu1);
	}
	
	ActionListener openSelectionListener = new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
			System.out.println("Overview: open ...");

			Rectangle selectedROI = null;
			String roiName = overview.getSelectedRoiName();
			
			if ( "all-roi".equals(roiName) ) {
				// Unrestricted -- OK: selectedROI == null;
			} 
			else if ( "my-roi".equals(roiName) ) {
				// My selected area
				Rectangle roi = overview.getImageStreamViewer().getDefROI();
				if (roi == null) {
					JOptionPane.showMessageDialog(null,
							"Please select the ROI on the panel",
							"Error", JOptionPane.ERROR_MESSAGE);
					return;
				}
				selectedROI = roi;
			} 
			else {
				// TODO: take ROI from the other options.
				// selectedROI = getRoi(roiName);
				// ...
			}
			
			StreamDef streamDef = overview.getSelectedStreamDef();
			if ( streamDef == null ) {
				JOptionPane.showMessageDialog(null,
						"Please select the desired stream", "Error",
						JOptionPane.ERROR_MESSAGE);
				return;
			}

			_openStream(streamDef, selectedROI);
			overview_jframe.setVisible(false);
		}
	};

	/** 
	  * Open the overview window for the user to select a ROI and
	  * a band to be dispatched in a tab in main window.
	  */
	public void showOverview() {
		if ( overview_jframe != null ) {
			overview_jframe.setVisible(true);
			return;
		}
		
		// get overview stream:
		StreamDef overviewStream = gscon.getOverviewStreamDef();
	
		if ( overviewStream == null ) {
			JOptionPane.showMessageDialog(
				null, 
				"Overview stream was not specified!\n"+
				"Please report this error.",
				"Error", 
				JOptionPane.ERROR_MESSAGE
			);
			return;
		}
		
		overview_jframe = new JFrame("Overview");

		ImageStreamViewer isv = overview.getImageStreamViewer();
		isv.writeConnectionStatus(Color.YELLOW, "Initializing");
		isv.setCanvasSize(canvasSize);
		isv.setStreamDef(overviewStream);
		isv.setROI(null);
		isv.setIsOverview(true);
		isv.setZoom(20);
		isv.setReferenceSpace(gscon.getReferenceSpace().getRectangle());
		
		overview_jframe.getContentPane().add(overview.getPanel());
		overview.getOpenButton().addActionListener(openSelectionListener);
		
		overview_jframe.pack();
		overview_jframe.setLocation(200,70);
		overview_jframe.setVisible(true);
		
		
		for ( StreamDef streamDef : gscon.getStreamDefs() ) {
			overview.addStreamDef(streamDef);
		}
		
		isv.setVectors(gscon.getVectors());

		// open stream for the overview:
		try {
			_addObserverAndOpenStream(overviewStream.getStreamID(), null);
			openImgStreams.put(overviewStream.getStreamID(), isv);
		}
		catch (Exception ex1) {
			System.out.println("cannot open stream for overview: " +ex1.getMessage());
			ex1.printStackTrace();
		}
	}
	
	/**
	 * @param streamID
	 * @param roi
	 * @throws Exception
	 */
	private void _addObserverAndOpenStream(String streamID, Rectangle roi) throws Exception {
		System.out.println("********* adding observer and opening streamID=" +streamID);
		gscon.addObserver(new ConnectionObserver(), streamID);
		gscon.openStream(streamID, roi);
	}

	/** stops all current streams */
	public void stop() {
		System.out.println("---- VizStreams: stop: ----");
		gscon.close();
		System.out.println("---- VizStreams stopped ----");
	}

	/** Starts the thread to get data from the stream. */
	private void _openStream(StreamDef streamDef, final Rectangle roi) {
		Thread t = _getOpenStreamThread(streamDef, roi);
		t.setPriority(Thread.NORM_PRIORITY);
		t.start();
	}
	
	/** Starts the thread to get data from the stream. */
	private Thread _getOpenStreamThread(final StreamDef streamDef, final Rectangle roi) {
		return new Thread(new Runnable() {
			public void run() {
				System.out.println("\nOpening stream: streamDef=" +streamDef+ " roi=" +roi);
				String error_msg = null;
				ImageStreamViewer isv = openImgStreams.get(streamDef.getStreamID());
				try {
					if ( isv == null ) {
						// create new stream:
						isv = new ImageStreamViewer();
						isv.writeConnectionStatus(Color.YELLOW, "Initializing");
						isv.setCanvasSize(canvasSize);
						isv.setStreamDef(streamDef);
						isv.setROI(roi);
						isv.setAutoSaveFrames(autoSaveFrames);
						isv.setReferenceSpace(referenceSpace);
						isv.imgPanel.setZoom(initialZoomFactor);
						tabs.addTab(isv.getShortName(), isv);
						tabs.setSelectedComponent(isv);
						openImgStreams.put(streamDef.getStreamID(), isv);
						isv.setVectors(gscon.getVectors());

						if ( !isv.isOverview() ) {
							String saved = gscon.getSavedURL(isv.streamDef.getStreamID());
							if ( saved != null
							&&   getLoadBackground()
							&&  !isv.saved_already_attempted ) {
								try {
									URL saved_url = new URL(saved);
									System.out.println("Loading " +saved_url);
									isv.loadBackgroundImage(saved_url);
								}
								catch(MalformedURLException ex) {
									System.out.println("MalformedException: " +ex.getMessage());
								}
								// even if an exception ocurred, we don't want to re-attempt
								isv.saved_already_attempted = true;
							}
						}
						
						_addObserverAndOpenStream(streamDef.getStreamID(), roi);
					}
					else {
						// check it's not the overview stream
						if ( !isv.isOverview() ) {
							tabs.setSelectedComponent(isv);
						}
					}
				}
				catch (OutOfMemoryError ex) {
					error_msg = "Error while creating image stream:\n"
							  + "The viewer cannot get the required system memory for this operation"
					;
				}
				catch (Throwable thr) {
					error_msg = "Error while creating image stream: "+thr.getMessage();
					thr.printStackTrace();
				}
				if ( error_msg != null ) {
					if ( isv != null ) {
						tabs.setSelectedComponent(isv);
						// make sure failed stream is interrupted:
						gscon.interruptStream(isv.getStreamDef().getStreamID());
					}
				}
				_updateButtons();
				
				if ( error_msg != null ) {
					System.out.println(error_msg);
					JOptionPane.showMessageDialog(
						null, 
						error_msg, 
						"Error", 
						JOptionPane.ERROR_MESSAGE
					);
				}
			}
		});
	}

	/** Saves the selected image. */
	private void _saveStream() {
		ImageStreamViewer isv = (ImageStreamViewer) tabs.getSelectedComponent();
		if ( isv != null ) {
			isv.save();
		}
	}
	
	/** Stops the selected stream. */
	private void _stopStream() {
		final ImageStreamViewer isv = (ImageStreamViewer) tabs.getSelectedComponent();
		if ( isv == null ) {
			return;
		}
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				gscon.interruptStream(isv.getStreamDef().getStreamID());
				_updateButtons();
			}
		});
	}

	/** Closes the selected stream. */
	private void _closeStream() {
		final ImageStreamViewer isv = (ImageStreamViewer) tabs.getSelectedComponent();
		if ( isv == null ) {
			return;
		}
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				isv.resetInfo();
				String streamID = isv.getStreamDef().getStreamID();
				gscon.interruptStream(streamID);
				openImgStreams.remove(streamID);
				tabs.remove(isv);
				_updateButtons();
			}
		});
	}

	class SaveAction extends AbstractAction {
		SaveAction() {
			super("Save");
			putValue(SHORT_DESCRIPTION, "Save current state of image (except lines) in selected tab");
			putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke("control S"));
			putValue(SMALL_ICON, _getIcon("Save24"));
		}
		public void actionPerformed(ActionEvent e) {
			_saveStream();
		}		
	}

	class StopAction extends AbstractAction {
		StopAction() {
			super("Stop");
			putValue(SHORT_DESCRIPTION, "Stop stream in selected tab");
			putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke("escape"));
			putValue(SMALL_ICON, _getIcon("Stop24"));
		}
		public void actionPerformed(ActionEvent e) {
			_stopStream();
		}		
	}

	class RoiAction extends AbstractAction {
		RoiAction() {
			super("ROI");
			putValue(SHORT_DESCRIPTION, "Options to select a region of interest");
			putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke("control r"));
			putValue(SMALL_ICON, _getIcon("Search24"));
		}
		public void actionPerformed(ActionEvent e) {
			popupRoi.show(buttonRoi, 5, buttonOpen.getHeight());
		}
	}

	class CloseAction extends AbstractAction {
		CloseAction() {
			super("Close");
			putValue(SHORT_DESCRIPTION, "Close selected tab");
			putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke("control W"));
			putValue(SMALL_ICON, _getIcon("Delete24"));
		}
		public void actionPerformed(ActionEvent e) {
			_closeStream();
		}		
	}
	
	class OpenAction  extends AbstractAction {
		OpenAction() {
			super("Open");
			putValue(SHORT_DESCRIPTION, "Select ROI and open a stream");
			putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke("control o"));
			putValue(SMALL_ICON, _getIcon("Open24"));
		}
		public void actionPerformed(ActionEvent e) {
			showOverview();
		}
	}

	public void setInitialZoomFactor(int initialZoomFactor) {
		this.initialZoomFactor = initialZoomFactor; 
	}

	class ConnectionObserver implements IConnectionObserver {
		public void newImage(IImage iimg) {
			if ( iimg.getStreamID().equals(gscon.getOverviewStreamDef().getStreamID()) ) {
				// update overview
				assert overview != null;
				ImageStreamViewer isv = overview.getImageStreamViewer();
				assert isv != null;
				iimg.setServerConnection(gscon);
				isv.addImage(iimg);
				isv.writeConnectionStatus(Color.GREEN, "Connected. Reading data...");
			}
			
			// get corresponding ImageStreamViewer
			ImageStreamViewer isv = openImgStreams.get(iimg.getStreamID());
			if (isv != null) {
				isv.addImage(iimg);
				isv.writeConnectionStatus(Color.GREEN, "Connected. Reading data...");
			}
		}
	
		public void frameStarts(FrameStatus frameStatus) {
			ImageStreamViewer isv = openImgStreams.get(frameStatus.getStreamDef().getStreamID());
			if (isv != null ) {
				isv.newFrame(frameStatus);
			}
			System.out.println("VizStreams: frameStarts: " +frameStatus);
		}
		
		public void frameEnds(FrameStatus frameStatus) {
			// TODO frameEnds
		}
		
		/**
		 * Returns true.
		 * TODO should return false, but for now returning true to maintain original
		 * behavior while I get time to review this code
		 */
		public boolean connectionException(String streamID, Exception ex) {
			ImageStreamViewer isv = openImgStreams.get(streamID);
			if (isv != null ) {
				isv.writeConnectionStatus(Color.RED, "Exception: " +ex.getMessage());
			}
			System.out.println("VizStreams: connectionException: " +ex.getMessage());
			return true;
		}
	
		public void connectedWaitingData(String streamID, String msg) {
			ImageStreamViewer isv = openImgStreams.get(streamID);
			if (isv != null ) {
				isv.writeConnectionStatus(Color.YELLOW, msg);
			}
		}
		
		/**
		 * Returns true.
		 * TODO should return false, but for now returning true to maintain original
		 * behavior while I get time to review this code
		 */
		public boolean readerFinished(final String streamID, final String msg) {
			EventQueue.invokeLater(new Runnable() {
				public void run() {
					ImageStreamViewer isv = openImgStreams.get(streamID);
					if (isv != null ) {
						isv.finishFrames();
						isv.writeConnectionStatus(Color.RED, msg);
					}
					_updateButtons();
				}
			});
			return true;
		}
	}


	/** 
	 * Standalone entry point.
	 * 
	 * Default parameters will connect to a geostreams repository:
	 * <pre>
	 * --gsSpec  http://geostreams.ucdavis.edu/~carueda/applet/dev/geostreams2.xml
	 * </pre>
	 * <p>
	 * --gsSpec can also be given as "host:port" to connect to a net-enabled workflow.
	 * 
	 * <p>
	 *
	 * Other options:
	 * <pre>
	 *	--auto-save-frames
	 *	--open-all-streams
	 *	--no-save-action
	 *	--no-developer-controls
	 *	--no-load-background
	 *	--bg-width <width of background image>   (by default, 2000)
	 *	--bg-height <hight of background image>   (by default, computed from width)
	 *  --no-aspect  TODO Fixes de size of the canvas image to 3067 x 1576   
	 * </pre>
	  */
	public static void main(String[] args) { 

		// parameters:
		String serverSpec = "http://geostreams.ucdavis.edu/~carueda/applet/dev/geostreams2.xml";

		boolean auto_save_frames = false;
		boolean open_all_streams = false;
		boolean include_dev_controls = true;
		boolean include_save_action = true;
		boolean load_background = true;
		boolean aspect_correction = true;

		// Desired background image size; see below.
		// By default bg_width=2000 and bg_height according to aspect:
		int bg_width =  2000;
		int bg_height = -1;   // can be user-given

		for ( int i = 0; i < args.length; i++ ) {
			if ( args[i].equals("--gsSpec") ) {
				serverSpec = args[++i];
			}
			else if ( args[i].equals("--auto-save-frames") ) {
				auto_save_frames = true;
			}
			else if ( args[i].equals("--open-all-streams") ) {
				open_all_streams = true;
			}
			else if ( args[i].equals("--no-developer-controls") ) {
				include_dev_controls = false;
			}
			else if ( args[i].equals("--no-save-action") ) {
				include_save_action = false;
			}
			else if ( args[i].equals("--no-load-background") ) {
				load_background = false;
			}
			else if ( args[i].equals("--bg-width") ) {
				bg_width = Integer.parseInt(args[++i]);
			}
			else if ( args[i].equals("--bg-height") ) {
				bg_height = Integer.parseInt(args[++i]);
			}
			else if ( args[i].equals("--no-aspect") ) {
				aspect_correction = false;
			}
			else if ( args[i].startsWith("-") ) {
				System.out.println("Unrecognized option: " +args[i]);
				return;
			}
		}
		
		System.out.println("Opening " +serverSpec);
		IServerConnection gscon; 
		try {
			gscon = ServerConnections.openConnection(serverSpec);
		}
		catch(ConnectException ex) {
			System.out.println("VizStreams: error connecting: " +ex.getClass().getCanonicalName()+ ": " +ex.getMessage());
			// no stacktrace
			return;
		}
		catch(Exception ex) {
			System.out.println("VizStreams: error connecting: " +ex.getClass().getCanonicalName()+ ": " +ex.getMessage());
			ex.printStackTrace();
			return;
		}
		
		if ( bg_height < 0 ) {
			if ( aspect_correction ) {
				bg_height = (int) (GOESConstants.aspect * bg_width);
			}
			else {
				bg_width  = 3067;     // FIXME temporary 
				bg_height = 1576;
			}
		}
		Dimension canvasSize = new Dimension(bg_width, bg_height);
		
		System.out.println("bg width x height = " +bg_width+ " x " +bg_height);
		System.out.println("auto_save_frames= " +auto_save_frames);
		System.out.println("open_all_streams= " +open_all_streams);
		
		final VizStreams geosViewer = new VizStreams(canvasSize);
		geosViewer.init(gscon);

		if ( include_save_action ) {
			geosViewer.includeSaveAction();
		}
		if ( include_dev_controls ) {
			geosViewer.includeDevControls();
		}
		setLoadBackground(load_background);
		geosViewer.setAutoSaveFrames(auto_save_frames);
		geosViewer.setOpenAllStreams(open_all_streams);

		final JFrame jframe = new JFrame("VizStreams - " +gscon.getServerShortDescription());

		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
					jframe.setSize(800,700);
					jframe.setLocation(100,25);
					jframe.getContentPane().add(geosViewer);
					jframe.setVisible(true);

					if ( !geosViewer.getOpenAllStreams() ) {
						geosViewer.showOverview();
					}
					geosViewer.start();
				} 
				catch (Exception e) {
					e.printStackTrace();
					JOptionPane.showMessageDialog(jframe, "Error starting viewer: " +e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
				}
			}
		});
	}


}

