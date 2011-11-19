package geostreams.ptolemy.icon;

import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.kernel.util.NamedObj;

/**
 * The icon for the image stream reader actor.
 * 
 * @author Carlos Rueda
 * @version $Id: ReaderIcon.java,v 1.11 2007/09/11 09:35:18 crueda Exp $
 */
public class ReaderIcon extends BaseIcon {
	public ReaderIcon(NamedObj container, String name)
            throws IllegalActionException, NameDuplicationException {
        super(container, name, "StreamReader");
    }

	protected void _createMonitorFigure() {
		final double radius = 4;
		_monitor.setTrafficLightFigure(3, (_heigth - 3*radius)/2, radius, 3*radius);
	}
}
