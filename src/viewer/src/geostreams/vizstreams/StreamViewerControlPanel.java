package geostreams.vizstreams;

import geostreams.image.VectorInfo;

import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamViewerControlPanel.java,v 1.2 2007/06/04 21:19:26 crueda Exp $
 */
class StreamViewerControlPanel extends JPanel {
	
	private StreamViewer streamViewer = null;

	private JButton viewButton = null;
	private JPopupMenu viewPopup = null;

	private JButton zoomButton = null;
	private JPopupMenu zoomPopup = null;

	private JSlider zoomSlider = null;


	private JCheckBoxMenuItem showFramesCheck;
	private JCheckBoxMenuItem showRasterImageCheck;

	private JLabel zoomLabel;

	private JButton clearButton = null;
	
	public StreamViewerControlPanel() {
		super();
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		this.add(getViewButton(), null);
		this.add(getZoomButton(), null);
		this.add(getClearButton(), null);
		//this.add(getZoomSlider(), null);
		//this.add(getZoomLabel(), null);
	}

	/**
	 * sets the associated imageStreamViewer
	 */
	void setStreamViewer(StreamViewer streamViewer) {
		this.streamViewer = streamViewer;
	}

	public void setIsOverview(boolean isOverview) {
		if ( isOverview ) {
			this.remove(getClearButton());
		}
	}

	/**
	 * This method initializes viewButton
	 * 
	 * @return javax.swing.JButton
	 */
	private JButton getViewButton() {
		if (viewButton == null) {
			viewButton = new JButton();
			viewButton.setMargin(new Insets(1,1,1,1));
			viewButton.setAlignmentX(0.5F);
			viewButton.setRequestFocusEnabled(false);
			viewButton.setIcon(new RotatedTextIcon(RotatedTextIcon.CCW,viewButton.getFont(),"View"));
			viewButton.setToolTipText("View options");
			viewButton.setEnabled(true);
			viewButton.addMouseListener(new java.awt.event.MouseAdapter() {
				public void mousePressed(java.awt.event.MouseEvent e) {
					if ( !getViewPopup().isVisible() ) {
						getViewPopup().show(viewButton, viewButton.getWidth()-3, 3);
					}
				}
			});
		}
		return viewButton;
	}

	/**
	 * This method initializes zoomButton
	 * 
	 * @return javax.swing.JButton
	 */
	private JButton getZoomButton() {
		if (zoomButton == null) {
			zoomButton = new JButton();
			zoomButton.setMargin(new Insets(1,1,1,1));
			zoomButton.setAlignmentX(0.5F);
			zoomButton.setRequestFocusEnabled(false);
			zoomButton.setIcon(new RotatedTextIcon(RotatedTextIcon.CCW,zoomButton.getFont(),"Zoom"));
			zoomButton.setToolTipText("Zoom factor");
			zoomButton.setEnabled(true);
			zoomButton.addMouseListener(new java.awt.event.MouseAdapter() {
				public void mousePressed(java.awt.event.MouseEvent e) {
					if ( !getZoomPopup().isVisible() ) {
						getZoomPopup().show(zoomButton, zoomButton.getWidth()-3, 3);
					}
				}
			});
		}
		return zoomButton;
	}

	/**
	 * This method initializes clearButton
	 * 
	 * @return javax.swing.JButton
	 */
	private JButton getClearButton() {
		if (clearButton == null) {
			clearButton = new JButton();
			clearButton.setMargin(new Insets(1,1,1,1));
			clearButton.setAlignmentX(0.5F);
			clearButton.setRequestFocusEnabled(false);
			clearButton.setIcon(new RotatedTextIcon(RotatedTextIcon.CCW,clearButton.getFont(),"Clear"));
			clearButton.setToolTipText("Clear the raster image in the display");
			clearButton.setEnabled(true);
			clearButton.addMouseListener(new java.awt.event.MouseAdapter() {
				public void mouseClicked(java.awt.event.MouseEvent e) {
					// NOTE java bug? this shouldn't be necessary, but the event is being 
					// delivered when the button is disabled!
					if ( clearButton.isEnabled() ) {
						streamViewer.imgPanel.clear();
						streamViewer.imgPanel.refresh();
					}
				}
			});
		}
		return clearButton;
	}
	
	/**
	 * This method initializes zoomSlider
	 * 
	 * @return javax.swing.JSlider
	 */
	JSlider getZoomSlider() {
		if (zoomSlider == null) {
			zoomSlider = new JSlider();
			zoomSlider.setOrientation(javax.swing.JSlider.HORIZONTAL);
			zoomSlider.setMaximum(200);
			zoomSlider.setMinimum(10);
			zoomSlider.setToolTipText("Zoom factor");
			//zoomSlider.setPreferredSize(new java.awt.Dimension(20,80));
			zoomSlider.addChangeListener(new ChangeListener() {
				public void stateChanged(ChangeEvent e) {
					JSlider source = (JSlider) e.getSource();
					int new_zoom = (int) source.getValue();
					streamViewer.eventFromSlider = true;
					streamViewer.imgPanel.setZoom(new_zoom);
					streamViewer.eventFromSlider = false;
				}
			});
		}
		return zoomSlider;
	}

	/**
	 * This method initializes zoomLabel
	 * 
	 * @return javax.swing.JLabel
	 */
	private JLabel getZoomLabel() {
		if (zoomLabel == null) {
			zoomLabel = new JLabel("Zoom", JLabel.CENTER);
		}
		return zoomLabel;
	}
	
	/**
	 * This method initializes viewPopup
	 * 
	 * @return javax.swing.JPopupMenu
	 */
	private JPopupMenu getViewPopup() {
		if (viewPopup == null) {
			viewPopup = new JPopupMenu();
			viewPopup.setLabel("View options");
			
			if ( streamViewer != null ) {
				Map<String,List<String>> groups = new LinkedHashMap<String,List<String>>(); 
				List<String> ungrouped = new ArrayList<String>();
				for (String vectorName: streamViewer.vectors.keySet()) {
					VectorInfo vectorInfo = streamViewer.vectors.get(vectorName);
					String group = vectorInfo.getGroup();
					if ( group == null ) {
						ungrouped.add(vectorName);
						continue;
					}
					List<String> vectorNames = groups.get(group);
					if ( vectorNames == null ) {
						groups.put(group, vectorNames = new ArrayList<String>());
					}
					vectorNames.add(vectorName);
					
				}
				
				for (String group : groups.keySet()) {
					List<String> vectorNames = groups.get(group);
					if ( vectorNames.size() > 1 ) {
						ButtonGroup buttonGroup = new ButtonGroup();
						for (String vectorName : vectorNames) {
							VectorInfo vectorInfo = streamViewer.vectors.get(vectorName);
							JMenuItem button = createVectorCheck(vectorInfo, true);
							buttonGroup.add(button);
							viewPopup.add(button);						
						}
						// add a radio button to not show any dataset in this group:
						JMenuItem button = createVectorCheck("No " +group, true);
						buttonGroup.add(button);
						viewPopup.add(button);						
					}
					else {
						String vectorName = vectorNames.get(0);
						VectorInfo vectorInfo = streamViewer.vectors.get(vectorName);
						viewPopup.add(createVectorCheck(vectorInfo, false));	
					}
					viewPopup.addSeparator();
				}

				for (String vectorName : ungrouped) {
					VectorInfo vectorInfo = streamViewer.vectors.get(vectorName);
					JMenuItem button = createVectorCheck(vectorInfo, false);
					viewPopup.add(button);						
				}
				viewPopup.addSeparator();
			}
			viewPopup.add(getShowFramesCheck());
			if ( !this.streamViewer.isOverview() ) {
				viewPopup.addSeparator();
				viewPopup.add(getShowRasterImageCheck());
			}
		}
		return viewPopup;
	}
	
	/**
	 * Forces the recreation of the view popup menu.
	 */
	public void refreshViewPopup() {
		viewPopup = null;
	}

	/**
	 * This method initializes zoomPopup
	 * 
	 * @return javax.swing.JPopupMenu
	 */
	private JPopupMenu getZoomPopup() {
		if (zoomPopup == null) {
			zoomPopup = new JPopupMenu();
			zoomPopup.setLabel("Zoom");
			zoomPopup.add(getZoomLabel());
			zoomPopup.add(getZoomSlider());
		}
		return zoomPopup;
	}

	
	private JMenuItem createVectorCheck(VectorInfo vectorInfo, boolean radio) {
		final String vectorName = vectorInfo.getName();
		final JMenuItem jmenuItem = radio ? new JRadioButtonMenuItem(vectorName) : new JCheckBoxMenuItem(vectorName);
		boolean select = vectorInfo.isSelected();
		streamViewer.setShowVector("vect:" +vectorName, select);
		jmenuItem.setSelected(streamViewer.imgPanel.isElementVisible("vect:" +vectorName));
		jmenuItem.addItemListener(new ItemListener() {
			public void itemStateChanged(ItemEvent e) {
				boolean selected = jmenuItem.isSelected();
				streamViewer.setShowVector(vectorName, selected);
				streamViewer.imgPanel.refresh();
			}
		});
		return jmenuItem;
	}

	private JMenuItem createVectorCheck(String label, boolean radio) {
		JMenuItem jmenuItem = radio ? new JRadioButtonMenuItem(label) : new JCheckBoxMenuItem(label);
		jmenuItem.setSelected(false);
		return jmenuItem;
	}
	
	private JCheckBoxMenuItem getShowFramesCheck() {
		if ( showFramesCheck == null ) {
			showFramesCheck = new JCheckBoxMenuItem("Frames and scan lines");
			showFramesCheck.setSelected(streamViewer.imgPanel.getShowFrames());
			showFramesCheck.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ev) {
					streamViewer.imgPanel.setShowFrames(showFramesCheck.isSelected());
					streamViewer.imgPanel.refresh();
				}
			});
		}
		return showFramesCheck;
	}

	private JCheckBoxMenuItem getShowRasterImageCheck() {
		if ( showRasterImageCheck == null ) {
			showRasterImageCheck = new JCheckBoxMenuItem("Show raster image");
			showRasterImageCheck.setSelected(streamViewer.imgPanel.getShowRasterImage());
			showRasterImageCheck.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ev) {
					streamViewer.imgPanel.setShowRasterImage(showRasterImageCheck.isSelected());
					streamViewer.imgPanel.refresh();
					clearButton.setEnabled(showRasterImageCheck.isSelected());
				}
			});
		}
		return showRasterImageCheck;
	}
	
	void updateZoomControls() {
		int zoom = streamViewer.imgPanel.getZoom();
		getZoomLabel().setText("Zoom: " +zoom+ "%");
		if (!streamViewer.eventFromSlider) {
			getZoomSlider().setValue(zoom);
		}
	}
}
