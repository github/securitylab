import cpp
import pointers.managed_ptr

/**
 * Long live classes, e.g. Singleton, classes owned by BrowserLoop etc.
 */

class OwnedByBrowserContext extends Class {
  OwnedByBrowserContext() {
    this.getName() = "VideoDecodePerfHistory" or
    this.getName() = "ServiceInstanceGroupHolder" or
    this.getName() = "BrowserContextServiceManagerConnectionHolder" or
    this.getName() = "ContentServiceDelegateHolder" or
    this.getName() = "FileServiceHolder" or
    this.getName() = "PermissionControllerImpl" or
    this.getName() = "BrowsingDataRemoverImpl" or
    this.getName() = "StoragePartitionImplMap" or
    this.getName() = "DownloadManager" or
    this.getName() = "BrowsingDataRemover" or
    this.getName() = "PermissionController"
  } 
}

class Singleton extends Type {
  Singleton() {
    this.hasName("MediaInternals") or
    hasName("NetworkConnectionTracker") or
    hasName("ChildProcessSecurityPolicyImpl") or
    hasName("MprisService") or
    hasName("PluginServiceFilter") or
    hasName("TickClock") or
    exists(Class c | c.getName().matches("Singleton<%") and
      this = c.getTemplateArgument(0)
    ) or
    exists(Variable v | v.isStatic() and
      v.getType().getName().matches("NoDestructor<%") and
      v.getType().(Class).getTemplateArgument(0) = this and
      not this.getName().matches("vector<%") and
      not this.getName().matches("atomic<%") and
      not this.getName().matches("set<%") and
      not this.getName().matches("basic_string<%") and
      not this.getName().matches("map<%") and
      not this.getName().matches("CallbackList<%") and
      not this.getName().matches("RepeatingCallback<%") and
      not this.getName().matches("OnceCallback<%") and
      not this.getName().matches("unique_ptr<%") and
      not this.getName().matches("scoped_refptr<%") and
      not this.getName().matches("WeakPtr<%") and
      not this.getName().matches("ThreadLocalPointer<%") and
      not this.getName().matches("ReceiverSetBase<%") and
      not this.getName().matches("SequenceLocalStorageSlot<%")
    )
  }
}

/**
 * Types that are owned by browser main loop. These types are usually long lived.
 */
class OwnedByBrowserMainLoop extends Class {
  OwnedByBrowserMainLoop() {
    exists(GeneralManagedField f | f.getManagedType() = this and
      f.getDeclaringType().hasName("BrowserMainLoop")
    ) or
    getName() = "UserInputMonitor" or
    getName() = "UserInputMonitorBase" or
    getName() = "BrowserGpuChannelHostFactory"
  }
}
