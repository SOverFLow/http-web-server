
/*
 * created by Devma at 21/10/2017
 *
*/

 $(document).ready(function(){
  $(".work-item > img").mouseenter(
   function(){
    $(this).next().css("display","block");
    $(this).next().slideDown("slow");

   });
   $(".work-item > img ").mouseleave (
    function(){
      $(this).next().css("display","none");
    });
    $(".more-details").mouseenter(
     function(){
       $(this).css("display","block");
     });
     $(".more-details").mouseleave(
      function(){
        $(this).css("display","none");
      });
// responsive
$(".icon").click(function(){
   $("#navbarid").toggleClass(" responsive");
 });

$("#about-me").click(function(){
  srollToPosition($(".about-me"))
});
$("#work").click(function(){
  srollToPosition($(".work"));
});
$("#contact").click(function(){
  srollToPosition($(".contact-us"));
});

  $(window).scroll(function(){
    hiddeArrowUp();
  });

 $(".goTop").click(function(){
   srollToPosition($("html"));
 });
//scroll to position
function srollToPosition(position){
  $("html,body").animate({
    scrollTop:position.offset().top
  },1500);
}

function hiddeArrowUp(){
  var currentPosition = $(window).scrollTop();
  if(currentPosition < 200){
  $(".goTop").hide();
  }
  else {
    $(".goTop").show();
  }
}
 });
