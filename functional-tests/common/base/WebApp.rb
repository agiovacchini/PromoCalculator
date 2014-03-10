class WebApp

   #@browser = ''
   @browserType = ''
   @loggedIn = 0
   @headlessLaunch = false
   @browserOpened = false 
   
   def initialize(pUrlBase, pBrowser, pHeadlessLaunch)
      @urlBase = pUrlBase
      @headlessLaunch = pHeadlessLaunch

      case pBrowser
        when 'ff'
          @browserType = :ff
        when 'chrome'
          @browserType = :chrome
      end

      if @headlessLaunch == true
         #@headless = Headless.new
         #@headless.start
         @browserType = :phantomjs
      end
   end

   def launch()
     puts "momama: #{@browserType}"
     @browser = Watir::Browser.new @browserType
     #, :service_log_path => 'kloy-chromedriver.log'

      @browser.goto @urlBase
      puts @browser.url + ' ' + @urlBase
      @browserOpened = true 
   end
   
   def close()
      #puts "Browser aperto: " << @browserOpened.to_s
      if @browserOpened
        @browser.close
        if @headlessLaunch == true
           @headless.destroy   
        end    
        @browserOpened = false
      end 
   end
   
end
