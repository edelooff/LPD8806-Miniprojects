#!/usr/bin/python
"""RedRunner output verification program."""
__author__ = 'Elmer de Looff <elmer.delooff@gmail.com>'
__version__ = '0.1'

# Standard modules
import re

MESSAGES = [
    (re.compile(r'\[RedRunner\]'), 'start'),
    (re.compile(r'H(\d+)'), 'verify_move'),
    (re.compile(r'Collided @ (\d+) \[(\d+)\]'), 'verify_collision'),
    (re.compile(r'Teleported to (\d+)'), 'verify_teleport'),
    (re.compile(r'Barrier @ (\d+)'), 'spawn_barrier'),
    (re.compile(r'Teleport @ (\d+),(\d+)'), 'spawn_teleport')]


class Error(Exception):
  """Base error class for the RedRunner verifier."""


class BadMessage(Error):
  """An unexpected message was received processing RedRunner output."""


class BadMove(Error):
  """An inconsistent move was detected parsing RedRunner output."""


class BadState(Error):
  """An inconsistent game state was found parsing RedRunner output."""


class RedRunner(object):
  """Simulation for the RedRunner LED-strip demo."""
  def __init__(self, track_length=None):
    """Initializes a RedRunner object."""
    self.direction = 1
    self.position = 0
    self.track_length = int(track_length) if track_length else None
    self.track_objects = {}
    self.new_objects = {}
    self.barrier_hitpoints = None
    self.updating_position = None
    self.started = False

  # ############################################################################
  # Message processing
  #
  def process(self, message):
    """Analyses a message and dispatches it to the handler method."""
    for pattern, method in MESSAGES:
      match = pattern.match(message)
      if match:
        return getattr(self, method)(*map(int, match.groups()))
    raise Error('Bad message: %r' % message)

  def start(self):
    """Marks RedRunner as started, errors when started again this run."""
    if self.started:
      raise BadState('RedRunner already started, cannot start again')
    self.started = True

  def verify_collision(self, position, initial_hitpoints):
    """Verifies that a reported barrier collision is correct.

    Errors are raised when the runner:
      * ... has skipped a previously placed object on the track;
      * ... runs into a barrier that is at the runner's position;
      * ... runs into a barrier that does not exist (or was destroyed);
      * ... runs into a barrier which reports its hitpoints wrongly.

    For reports that update position, the move into the barrier position is
    checked. For reports that do not update position, the path of the runner
    from the last known position is checked for pre-existing objects.
    """
    if self.barrier_hitpoints is None:
      self.set_barrier_hitpoints(initial_hitpoints)
    if self.updating_position:
      self.verify_move(position)
    else:
      if self.first_object_until(position):
        raise BadMove(self.error_missed(position))
    barrier = self.track_objects.pop(position, None)
    if not isinstance(barrier, Barrier):
      raise BadMove('Crashed into nonexisting barrier at %d.' % position)
    if barrier.hitpoints != initial_hitpoints:
      raise BadState('Barrier has %d hitpoints, not the reported %d.' % (
          barrier.hitpoints, initial_hitpoints))
    if barrier.hit():
      self.track_objects[position] = barrier
    self.direction *= -1
    self.step_one()

  def verify_move(self, expected):
    """Verifies that the reported move is correct.

    This method also detects looping around the track, determining the track
    length whenever this first occurs. When the track length is established,
    the bounding for this is also checked.
    """
    self.updating_position = True
    calculated = self.step_one()
    if expected != calculated:
      if self.track_length is None and (expected == 0 or calculated == -1):
        self.track_length = expected + abs(calculated)
        self.position = expected
      else:
        raise BadMove('Calculated position %d does not match reported %d.' % (
            calculated, expected))

  def verify_teleport(self, destination):
    """Verifies that a reported teleport is correct.

    This checks that the both the target and originating teleport exist and
    move the runner from the correct position to the intended destination.

    For reports that do not update position, the path of the runner from the
    last known position is checked for pre-existing objects.
    """
    tp_return = self.track_objects.pop(destination, None)
    if not self.updating_position:
      if self.first_object_until(tp_return.destination):
        raise BadMove(self.error_missed(tp_return.destination))
    if not isinstance(tp_return, Teleport):
      raise BadMove('No receiving teleporter at position %d.' % destination)
    tp_origin = self.track_objects.pop(tp_return.destination, None)
    if not isinstance(tp_origin, Teleport):
      raise BadMove('No originating teleporter at position %d.' % self.position)
    self.position = destination

  def spawn_barrier(self, position):
    """Adds a barrier onto the track in the given position."""
    if position in self.track_objects:
      raise BadMove(self.error_placement(position, 'Barrier'))
    self.add_object(position, Barrier(self.barrier_hitpoints))

  def spawn_teleport(self, *positions):
    """Adds a teleport onto the track in the given position."""
    for origin, destination in zip(positions, reversed(positions)):
      if origin in self.track_objects:
        raise BadMove(self.error_placement(origin, 'Teleport'))
      self.add_object(origin, Teleport(destination))

  # ############################################################################
  # Errors and configuration settings
  #
  def error_missed(self, position):
    """Returns an informative missed object error string."""
    return 'Ran into %r at %d but should have hit %r first.' % (
        self.track_objects[position],
        position,
        self.track_objects[self.position])

  def error_placement(self, position, object_type):
    """Returns an informative placement error string."""
    return 'Cannot place %s at %d, already contains %r.' % (
        object_type, position, self.track_objects[position])

  def add_object(self, position, obj):
    """Adds an object to the track.

    If position is not tracked, it's also added to a mapping with 'new' objects,
    which may be placed after the runner passes that position. This mapping is
    emptied whenever the location of the runner is known.
    """
    self.track_objects[position] = obj
    if not self.updating_position:
      self.new_objects[position] = obj

  def first_object_until(self, target):
    """Returns the first object encountered on the track up to `position`.

    If this method does return, this usually indicates an unexpected object was
    on the track.
    """
    current = self.step_one()
    while current != target:
      if current in self.track_objects and current not in self.new_objects:
        return self.track_objects[current]
      current = self.step_one()
    self.new_objects.clear()

  def set_barrier_hitpoints(self, hitpoints):
    """Updates the hitpoints on all barriers on the track."""
    self.barrier_hitpoints = hitpoints
    for track_object in self.track_objects.values():
      if isinstance(track_object, Barrier):
        track_object.hitpoints = hitpoints

  def step_one(self):
    """Moves the runner forward by one step.

    This method will loop the runner back around the other side of the track if
    it moves off of either end of the track.
    """
    self.position += self.direction
    if self.track_length is not None:
      self.position %= self.track_length
    elif not self.updating_position and abs(self.position) > 9000:
      raise Error('Position over 9000, specify track_length if this is correct')
    return self.position


class Barrier(object):
  """Barriers in RedRunner cause the runner to bounce off.

  They have a number of hitpoints. Once these are reduced to zero, the barrier
  disappears from the track.
  """
  def __init__(self, hitpoints):
    """Initializes a Barrier object."""
    self.hitpoints = hitpoints

  def __repr__(self):
    """Object representation."""
    return '%s(hitpoints=%d)' % (type(self).__name__, self.hitpoints)

  def hit(self):
    """Registers a hit on the Barrier and returns the remaining hitpoints."""
    self.hitpoints -= 1
    return self.hitpoints


class Teleport(object):
  """Teleports exist in pairs and move the runner from one to the other.

  After the teleportation, both teleports disappear from the track.
  """
  def __init__(self, destination):
    """Initializes a Teleport object."""
    self.destination = destination

  def __repr__(self):
    """Object representation."""
    return '%s(destination=%d)' % (type(self).__name__, self.destination)


def verify(filename, track_length=None):
  """Verifies the correctness of a RedRunner log."""
  red_runner = RedRunner(track_length=track_length)
  with file(filename) as log:
    for index, message in enumerate(log, 1):
      message = message.strip()
      print '%d) %s' % (index, message)
      red_runner.process(message)


if __name__ == '__main__':
  import sys
  if len(sys.argv) != 2:
    print 'Usage: %s INPUT_FILE' % sys.argv[0]

  verify(*sys.argv[1:])
