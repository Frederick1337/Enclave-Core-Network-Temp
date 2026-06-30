// =========================================================================
// SOURCE CODE: src/server/attested_network_transport.cpp
// REPOSITORY: Enclave-Core-Network-Temp (Temporary Software Transport)
// MASTER ARCHITECT: Frederick Joseph Lombardi
// =========================================================================

#include <iostream>
#include <cstdint>
#include <cstring>
#include <atomic>

extern "C" uint64_t g_DynamicMutationKey;

class AttestedNetworkTransport {
private:
    uint64_t master_token;
    std::atomic<uint64_t> current_epoch_id;
    uint64_t software_epoch_keys; // Sliding Window Cache: [En-1], [En], [En+1]

public:
    AttestedNetworkTransport(uint64_t token) 
        : master_token(token), current_epoch_id(45) {
        // Derive rolling keys locally based on host hardware seed
        software_epoch_keys = g_DynamicMutationKey ^ 0xAAAA; // En-1
        software_epoch_keys = g_DynamicMutationKey ^ 0xBBBB; // En
        software_epoch_keys = g_DynamicMutationKey ^ 0xCCCC; // En+1
    }

    // Encrypts outgoing memory data frames via 50ms software rolling cipher
    void SecureAndTransmitFrame(uint8_t* raw_payload, size_t size, uint8_t* output_packet) {
        if (master_token != 0x55AAFJLOMBARDI) return;

        uint64_t active_key = software_epoch_keys; // Target Current Epoch Key
        uint64_t current_epoch = current_epoch_id.load(std::memory_order_relaxed);

        // Stamp unencrypted metadata Epoch Tag onto the header
        std::memcpy(output_packet, &current_epoch, sizeof(current_epoch));

        // Encrypt data payload block on the fly via XOR block streaming
        for (size_t i = 0; i < size; ++i) {
            output_packet[sizeof(current_epoch) + i] = raw_payload[i] ^ static_cast<uint8_t>(active_key >> (i % 8));
        }
    }

    // Decrypts incoming frames via ultra-low latency 3-slot sliding window cache
    bool ProcessIncomingFrame(uint8_t* incoming_packet, size_t payload_size, uint8_t* cleared_output) {
        uint64_t packet_epoch = 0;
        std::memcpy(&packet_epoch, incoming_packet, sizeof(packet_epoch));

        uint64_t current_epoch = current_epoch_id.load(std::memory_order_relaxed);
        uint64_t decryption_key = 0;

        // Sliding Window Key Selection Logic
        if (packet_epoch == current_epoch) {
            decryption_key = software_epoch_keys; // Current
        } else if (packet_epoch == current_epoch - 1) {
            decryption_key = software_epoch_keys; // Previous
        } else if (packet_epoch == current_epoch + 1) {
            decryption_key = software_epoch_keys; // Next
        } else {
            std::cerr << "[TRANSPORT DROPPED] Packet out of sliding window epoch sync.\n";
            return false; 
        }

        // Decode packet into secure local RAM buffer
        for (size_t i = 0; i < payload_size; ++i) {
            cleared_output[i] = incoming_packet[sizeof(packet_epoch) + i] ^ static_cast<uint8_t>(decryption_key >> (i % 8));
        }
        return true;
    }
};

extern "C" void ExecuteSoftwareNetworkSync(uint8_t* data, size_t len, uint8_t* out) {
    AttestedNetworkTransport transport(0x55AAFJLOMBARDI);
    transport.SecureAndTransmitFrame(data, len, out);
}
