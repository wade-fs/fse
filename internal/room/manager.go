package room

import "sync"

type Room struct {
	ID    string
	Users map[string]bool
}

type Manager struct {
	mutex sync.RWMutex
	rooms map[string]*Room
}

func NewManager() *Manager {

	return &Manager{
		rooms: map[string]*Room{},
	}
}

func (m *Manager) Join(roomID string, userID string) {

	m.mutex.Lock()
	defer m.mutex.Unlock()

	room, ok := m.rooms[roomID]

	if !ok {

		room = &Room{
			ID:    roomID,
			Users: map[string]bool{},
		}

		m.rooms[roomID] = room
	}

	room.Users[userID] = true
}
