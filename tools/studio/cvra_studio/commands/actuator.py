import argparse
import datetime
import sys
import threading
import time

import uavcan

from pyqtgraph.Qt import QtCore, QtGui
from ..network.UavcanNode import UavcanNode
from ..viewers.wrappers import LineEdit, ComboBox
from ..viewers.helpers import vstack, hstack


def argparser(parser=None):
    parser = parser or argparse.ArgumentParser(description=__doc__)
    parser.add_argument("interface", help="Serial port or SocketCAN interface")
    parser.add_argument("--dsdl", "-d", help="DSDL path", required=True)
    parser.add_argument("--node_id", "-n", help="UAVCAN Node ID", default=127)
    parser.add_argument("board_id", help="Target board ID", type=int)

    return parser


class ServoInputWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(ServoInputWidget, self).__init__(parent)

        self.position, self.acceleration, self.velocity = 0.05, 0.1, 0.1

        def change_callback(_):
            self.position = float(self.pos.text())
            self.acceleration = float(self.acc.text())
            self.velocity = float(self.vel.text())

        def s(val):
            return "{:.2f}".format(val)

        self.pos = LineEdit(
            title="Duty Cycle (0-1)",
            parent=parent,
            initial_value=s(self.position),
            callback=change_callback,
        )
        self.vel = LineEdit(
            title="Velocity",
            parent=parent,
            initial_value=s(self.velocity),
            callback=change_callback,
        )
        self.acc = LineEdit(
            title="Acceleration",
            parent=parent,
            initial_value=s(self.acceleration),
            callback=change_callback,
        )

        self.setLayout(vstack([self.pos, self.vel, self.acc,]))
        self.show()


class PumpInputWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(PumpInputWidget, self).__init__(parent)

        self.duty_cycle = 0

        def change_callback(_):
            self.duty_cycle = float(self._duty_cycle_widget.text())

        self._duty_cycle_widget = LineEdit(
            title="Duty Cycle (0-1)",
            parent=parent,
            initial_value="0",
            callback=change_callback,
        )
        self._solenoid_widget = QtGui.QCheckBox("Solenoid", parent=self)

        self.setLayout(vstack([self._duty_cycle_widget, self._solenoid_widget]))
        self.show()

    def solenoid_enabled(self):
        return bool(self._solenoid_widget.checkState())


class StatusOutputWidget(QtGui.QWidget):
    def __init__(self, parent=None):
        super(StatusOutputWidget, self).__init__(parent)
        self._uptime_widget = LineEdit(
            title="Uptime [s]",
            parent=parent,
            initial_value="0",
        )
        self._uptime_widget.line.readOnly = True
        self._health_widget = LineEdit(
            title="Node health",
            parent=parent,
            initial_value="0",
        )
        self._health_widget.line.readOnly = True

        self.setLayout(vstack([self._uptime_widget, self._health_widget]))
        self.show()

    def set_uptime(self, uptime):
        self._uptime_widget.line.setText(str(uptime))

    def set_health(self, health):
        health_map = {0: "OK", 1: "WARNING", 2: "ERROR", 3: "CRITICAL"}
        health = health_map.get(health, "UNKNOWN")
        self._health_widget.line.setText(health)


class ActuatorBoardView(QtGui.QWidget):
    def __init__(self, parent=None):
        super(ActuatorBoardView, self).__init__(parent)

        self.setWindowTitle("Actuator board")

        self.servo = [ServoInputWidget(parent) for _ in range(2)]
        self.servo_box = [
            QtGui.QGroupBox("Servo {}".format(i + 1)) for i in range(len(self.servo))
        ]
        for s, sb in zip(self.servo, self.servo_box):
            sb.setLayout(vstack([s]))

        self.pump = [PumpInputWidget(parent) for _ in range(2)]
        self.pump_box = [
            QtGui.QGroupBox("Pump {}".format(i + 1)) for i in range(len(self.pump))
        ]
        for s, sb in zip(self.pump, self.pump_box):
            sb.setLayout(vstack([s]))

        self.status = StatusOutputWidget(parent)
        self.status_box = QtGui.QGroupBox("Node status")
        self.status_box.setLayout(vstack([self.status]))

        self.setLayout(vstack([hstack(self.servo_box), hstack(self.pump_box), self.status_box]))
        self.show()


class ActuatorBoardController:
    def __init__(self, view, node, dst_id):
        self.node = node
        self.view = view
        self.dst_id = dst_id
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self._send)
        node.add_handler(uavcan.protocol.NodeStatus, self._node_status_callback)

    def start(self):
        self.timer.start(100)

    def _node_status_callback(self, event):
        if event.transfer.source_node_id != self.dst_id:
            return

        self.view.status.set_uptime(event.message.uptime_sec)
        self.view.status.set_health(event.message.health)

    def _send(self):
        try:
            servo = [
                uavcan.thirdparty.cvra.ServoTrajectory(
                    position=s.position,
                    velocity=s.velocity,
                    acceleration=s.acceleration,
                )
                for s in self.view.servo
            ]
            pump = [p.duty_cycle for p in self.view.pump]
            solenoid = [p.solenoid_enabled() for p in self.view.pump]

            msg = uavcan.thirdparty.cvra.actuator.Command(
                node_id=self.dst_id,
                servo_trajectories=servo,
                pump=pump,
                solenoid=solenoid,
            )
        except ValueError:
            return

        self.node.broadcast(msg)


def main(args):
    app = QtGui.QApplication(sys.argv)
    uavcan.load_dsdl(args.dsdl)
    view = ActuatorBoardView()
    view.show()

    node = UavcanNode(interface=args.interface, node_id=args.node_id)
    node.spin()

    controller = ActuatorBoardController(view, node, args.board_id)
    controller.start()

    app.exec_()


if __name__ == "__main__":
    args = argparser().parse_args()
    main(args)
