package geostreams.ptolemy.icon;

import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;
import ptolemy.kernel.util.NamedObj;

/**
 * Adapted from BoxedIcon
 * @author Carlos Rueda
 * @version $Id: ViewerIcon.java,v 1.8 2007/06/26 20:53:08 crueda Exp $
 */
public class ViewerIcon extends BaseIcon {
    public ViewerIcon(NamedObj container, String name)
            throws IllegalActionException, NameDuplicationException {
        super(container, name, "Viewer");
    }

    protected void _createMonitorFigure() {
		final double radius = 4;
		_monitor.setTrafficLightFigure(
//				_width - 6,
				7,
				(_heigth - 3*radius)/2, radius, 3*radius);
	}
}
