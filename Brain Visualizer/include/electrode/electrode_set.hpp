#pragma once

#include <electrode/electrode_system.hpp>
#include <electrode/electrode.hpp>

namespace brainviz::electrode
{
   /**
    * @brief A wrapper class for managing electrode sets from a specific system
    *
    * This class provides convenient access to a set of electrodes from a specific system type
    * and offers helper methods for electrode lookup and iteration.
    */
   class ElectrodeSet
   {
   public:
      /**
       * @brief Construct a new electrode set from a system type
       * @param type The electrode system type to use (16, 32, 64, 128, or 256 channels)
       */
      explicit ElectrodeSet(const SystemType type)
         : m_type(type), m_electrodes(ElectrodeSystem::get(type))
      {
      }

      /**
       * @brief Get all electrodes in the set
       * @return A span containing all electrodes in this set
       */
      [[nodiscard]] std::span<const Electrode> all() const noexcept
      {
         return m_electrodes;
      }

      /**
       * @brief Get the system type of this electrode set
       * @return The electrode system type
       */
      [[nodiscard]] SystemType type() const noexcept
      {
         return m_type;
      }

      /**
       * @brief Find an electrode by name within this set
       * @param name The electrode name to find
       * @return The electrode if found, otherwise an error message
       */
      [[nodiscard]] std::expected<Electrode, std::string> find(const std::string_view name) const
      {
         return ElectrodeSystem::find_by_name(m_type, name);
      }

      /**
       * @brief Find an electrode by ID within this set
       * @param id The electrode ID to find
       * @return The electrode if found, otherwise an error message
       */
      [[nodiscard]] std::expected<Electrode, std::string> find(const int id) const
      {
         return ElectrodeSystem::find_by_id(m_type, id);
      }

      /**
       * @brief Get the number of electrodes in this set
       * @return The number of electrodes
       */
      [[nodiscard]] size_t size() const noexcept
      {
         return m_electrodes.size();
      }

      /**
       * @brief Get iterator to the beginning of the electrode set
       * @return Iterator to the first electrode
       */
      [[nodiscard]] auto begin() const noexcept
      {
         return m_electrodes.begin();
      }

      /**
       * @brief Get iterator to the end of the electrode set
       * @return Iterator to the position after the last electrode
       */
      [[nodiscard]] auto end() const noexcept
      {
         return m_electrodes.end();
      }

   private:
      SystemType m_type;
      std::span<const Electrode> m_electrodes;
   };
}
