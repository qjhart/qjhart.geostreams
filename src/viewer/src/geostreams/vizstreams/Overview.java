package geostreams.vizstreams;

import geostreams.image.StreamDef;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.LayoutManager;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.AbstractButton;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

/**
 * The overview panel for the user to select a stream and ROI to be opened in
 * main window.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: Overview.java,v 1.4 2007/06/02 19:46:17 crueda Exp $
 */
public abstract class Overview {
	
	/**
	 * Creates an overview.
	 * @return
	 */
	public static Overview createOverview() {
		return new OV2();
	}

	protected JPanel thePanel = new JPanel();

	protected JButton openButton = null;

	protected ImageStreamViewer imageStreamViewer = null;

	protected JPanel choosePanel = null;

	protected JPanel optionsPanel = null;

	protected JCheckBox loadBackgroundCheck = null;

	protected Overview() {
		thePanel.setLayout(new BorderLayout());
	}
	
	/**
	 * This method initializes choosePanel
	 * 
	 * @return javax.swing.JPanel
	 */
	protected JPanel getChoosePanel() {
		if (choosePanel == null) {
			choosePanel = new JPanel();
			choosePanel.setLayout(new GridLayout(1, 0));
			choosePanel.add(getOptionsPanel(), null);
			
			choosePanel.setBorder(BorderFactory.createTitledBorder(null, ""));

		}
		return choosePanel;
	}

	/**
	 * This method initializes jCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	protected JCheckBox getLoadBackgroundCheck() {
		if (loadBackgroundCheck == null) {
			loadBackgroundCheck = new JCheckBox();
			loadBackgroundCheck.setSelected(VizStreams.getLoadBackground());
			loadBackgroundCheck.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
			loadBackgroundCheck.setToolTipText("Check this to pre-load a recent image of the stream");
			loadBackgroundCheck.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					VizStreams.setLoadBackground(loadBackgroundCheck.isSelected());
				}
			});
			loadBackgroundCheck.setText("Load background");
		}
		return loadBackgroundCheck;
	}

	/**
	 * @return help text
	 */
	protected String getHelpText() {
		return "<html>"
			+ "Status of the current sector being scanned by the satellite<br>"
			+ "instrument. No image data is actually visualized here.<br>"
			+ "To visualize data, select a region of interest, a stream,<br>"
			+ "and click the Open button."
			+ "<br>"
			+ "</html>"
		;
	}
	
	/** 
	 * Gets the graphical component containing the overview panel.
	 */
	public Component getPanel() {
		return thePanel;
	}

	/**
	 * Returns the openButton
	 * 
	 * @return javax.swing.JButton
	 */
	public JButton getOpenButton() {
		if (openButton == null) {
			openButton = new JButton();
			openButton.setText("Open");
			openButton.setToolTipText("Click to open the selected stream");
		}
		return openButton;
	}


	/**
	 * This method initializes imageStreamViewer
	 * 
	 * @return geostreams.vizstreams.ImageStreamViewer
	 */
	public ImageStreamViewer getImageStreamViewer() {
		if (imageStreamViewer == null) {
			imageStreamViewer = new ImageStreamViewer();
			imageStreamViewer.setIsOverview(true);
		}
		return imageStreamViewer;
	}

	protected abstract JPanel getOptionsPanel();
	abstract String getSelectedRoiName();
	abstract StreamDef getSelectedStreamDef();
	abstract void addStreamDef(StreamDef streamDef);
	
	

	private static class OV2 extends Overview {
		
		private JPanel streamPanel = null;
		private ButtonGroup streamGroup;
		private StreamDef selectedStreamDef;

		private JPanel roiPanel = null;
		private ButtonGroup roiGroup;
		private String selectedRoiName;
		
		OV2() {
			thePanel.add(getImageStreamViewer(), java.awt.BorderLayout.CENTER);
			thePanel.add(getChoosePanel(), java.awt.BorderLayout.WEST);
			thePanel.setPreferredSize(new Dimension(700, 400));
		}
		
		
		protected JPanel getOptionsPanel() {
			if ( optionsPanel == null ) {
				optionsPanel = new JPanel();
				LayoutManager lm = new BoxLayout(optionsPanel, BoxLayout.Y_AXIS);
				optionsPanel.setLayout(lm);
				optionsPanel.add(new JLabel(getHelpText()));
				optionsPanel.add(new JLabel(" ")); // just a filler
				optionsPanel.add(getRoiPanel());
				optionsPanel.add(getStreamPanel());
				optionsPanel.add(getLoadBackgroundCheck());
				optionsPanel.add(getOpenButton());
			}
			return optionsPanel;
		}

		private JPanel getRoiPanel() {
			if ( roiPanel == null ) {
				roiPanel = new JPanel();
				LayoutManager lm = new BoxLayout(roiPanel, BoxLayout.Y_AXIS);
				roiPanel.setLayout(lm);
				roiGroup = new ButtonGroup();
				
				addRoiName("all-roi", "Unrestricted", 
						"All data in the stream will be displayed", true
				);
				addRoiName("my-roi", "My ROI drawn on the panel",
						"Drag the mouse while holding down the Shift key", false
				);
				
				roiPanel.setBorder(BorderFactory.createTitledBorder(null, "Select ROI"));

			}
			return roiPanel;
		}
		
		private void addRoiName(final String roiName, String label, String tip, boolean selected) {
			final AbstractButton cb = new JRadioButton(label, selected);
			if ( selected ) {
				selectedRoiName = roiName;
			}
			cb.setToolTipText(tip);
			cb.addItemListener(new ItemListener() {
				public void itemStateChanged(ItemEvent e) {
					if ( cb.isSelected() ) {
						selectedRoiName = roiName;
					}
				}
			});
			roiGroup.add(cb);
			roiPanel.add(cb);
		}
		
		public String getSelectedRoiName() {
			return selectedRoiName;
		}

		private JPanel getStreamPanel() {
			if ( streamPanel == null ) {
				streamPanel = new JPanel();
				LayoutManager lm = new BoxLayout(streamPanel, BoxLayout.Y_AXIS);
				streamPanel.setLayout(lm);
				streamGroup = new ButtonGroup();
				
				streamPanel.setBorder(BorderFactory.createTitledBorder(null, "Select stream"));
				
			}
			return streamPanel;
		}
		
		public void addStreamDef(final StreamDef streamDef) {
			final AbstractButton cb = new JRadioButton(streamDef.getName());
			cb.addItemListener(new ItemListener() {
				public void itemStateChanged(ItemEvent e) {
					if ( cb.isSelected() ) {
						selectedStreamDef = streamDef;
					}
				}
			});
			streamGroup.add(cb);
			streamPanel.add(cb);
			if ( streamGroup.getButtonCount() == 1 ) {
				cb.setSelected(true);
			}
		}
		
		public StreamDef getSelectedStreamDef() {
			return selectedStreamDef;
		}
	}
}
